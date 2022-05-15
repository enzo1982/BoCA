 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#ifdef __WIN32__
#	include <windows.h>
#else
#	include <sys/mman.h>
#	include <sys/sem.h>
#	include <sys/wait.h>
#	include <stdlib.h>
#	include <unistd.h>
#	include <fcntl.h>
#	include <pthread.h>
#endif

#include "coreaudioconnect.h"
#include "dllinterface.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderCoreAudioConnect::GetComponentSpecs()
{
	static String	 componentSpecs;

#ifndef __WIN32__
	const Array<String>	&args = GUI::Application::GetArguments();

	foreach (const String &arg, args)
	{
		if (arg == "--disable-wine") return componentSpecs;
	}
#endif

	if (EncoderCoreAudioConnect().IsReady())
	{
		componentSpecs = "									\
													\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>						\
		  <component>										\
		    <name>Core Audio AAC/ALAC Encoder</name>						\
		    <version>1.0</version>								\
		    <id>coreaudio-enc</id>								\
		    <type>encoder</type>								\
		    <replace>faac-enc</replace>								\
		    <replace>voaacenc-enc</replace>							\
		    <replace>ffmpeg-alac-enc</replace>							\
		    <format>										\
		      <name>MPEG-4 AAC Files</name>							\
		      <extension>m4a</extension>							\
		      <extension>m4b</extension>							\
		      <extension>m4r</extension>							\
		      <extension>mp4</extension>							\
		      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>				\
		    </format>										\
		    <format>										\
		      <name>Raw AAC Files</name>							\
		      <extension>aac</extension>							\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>				\
		    </format>										\
		    <format>										\
		      <name>Apple Lossless Files</name>							\
		      <extension>m4a</extension>							\
		      <extension>m4b</extension>							\
		      <extension>mp4</extension>							\
		      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>				\
		    </format>										\
		    <parameters>									\
		      <selection name=\"Output format\" argument=\"-f %VALUE\" default=\"AAC\">		\
			<option alias=\"Advanced Audio Coding\">AAC</option>				\
			<option alias=\"Apple Lossless Audio Codec\">ALAC</option>			\
		      </selection>									\
		      <range name=\"Bitrate per channel\" argument=\"-b %VALUE\" default=\"64\">	\
			<min alias=\"min\">8</min>							\
			<max alias=\"max\">192</max>							\
		      </range>										\
		      <switch name=\"Write raw AAC files\" argument=\"--raw\"/>				\
		    </parameters>									\
		  </component>										\
													\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadMP4v2DLL();
}

Void smooth::DetachDLL()
{
	FreeMP4v2DLL();
}

BoCA::EncoderCoreAudioConnect::EncoderCoreAudioConnect()
{
#ifdef __WIN32__
	connector   = NIL;
	mapping	    = NIL;
#else
	connector   = -1;
	mapping	    = -1;
#endif

	comm	    = NIL;
	connected   = False;
	ready	    = False;

	fileType    = 0;

	configLayer = NIL;
	config	    = NIL;
}

BoCA::EncoderCoreAudioConnect::~EncoderCoreAudioConnect()
{
	if (connected) Disconnect();

	if (config != NIL) Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderCoreAudioConnect::IsReady() const
{
	if (!connected) const_cast<EncoderCoreAudioConnect *>(this)->Connect();

	return ready;
}

Bool BoCA::EncoderCoreAudioConnect::IsLossless() const
{
	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	Int	 codec = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);

	/* Signal lossless for ALAC.
	 */
	if (codec == CA::kAudioFormatAppleLossless) return True;

	return False;
}

Bool BoCA::EncoderCoreAudioConnect::Activate()
{
	const Format	&format = track.GetFormat();

	/* Get configuration.
	 */
	config = Config::Copy(GetConfiguration());

	ConvertArguments(config);

	Int	 codec	      = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);
	Int	 kbps	      = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Bitrate", 64);
	Bool	 mp4Container = config->GetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", True);

	/* Get file type of output file.
	 */
	fileType = mp4Container ? CA::kAudioFileM4AType : CA::kAudioFileAAC_ADTSType;

	/* Write ID3v2 tag if requested.
	 */
	if (fileType == CA::kAudioFileAAC_ADTSType && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureCoreAudio::ConfigID, "AllowID3v2", False))
	{
		const Info	&info = track.GetInfo();

		if (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->SetConfiguration(config);
				tagger->RenderBuffer(id3Buffer, track);

				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	driver->Close();

	/* Send Setup command.
	 */
	if (!connected) Connect();

	comm->command = CommCommandSetup;
	comm->length  = sizeof(CoreAudioCommSetup);

	((CoreAudioCommSetup *) &comm->data)->codec    = codec;
	((CoreAudioCommSetup *) &comm->data)->bitrate  = kbps * 1000 * format.channels;
	((CoreAudioCommSetup *) &comm->data)->format   = mp4Container;

	((CoreAudioCommSetup *) &comm->data)->channels = format.channels;
	((CoreAudioCommSetup *) &comm->data)->rate     = format.rate;
	((CoreAudioCommSetup *) &comm->data)->bits     = format.bits;

	((CoreAudioCommSetup *) &comm->data)->fp       = format.fp;
	((CoreAudioCommSetup *) &comm->data)->sign     = format.sign;

	char	*outfile = track.outputFile.ConvertTo("UTF-8");

	memcpy(((CoreAudioCommSetup *) &comm->data)->file, outfile, strlen(outfile) + 1);

	ProcessConnectorCommand();

	if (comm->status != CommStatusReady) return False;

	return True;
}

Bool BoCA::EncoderCoreAudioConnect::Deactivate()
{
	/* Send Finish command.
	 */
	if (!connected) Connect();

	comm->command = CommCommandFinish;
	comm->length  = 0;

	ProcessConnectorCommand();

	if (comm->status != CommStatusReady) return False;

	/* Finish MP4 writing.
	 */
	if (fileType == CA::kAudioFileM4AType)
	{
		/* Get total duration.
		 */
		comm->command = CommCommandDuration;
		comm->length  = 0;

		ProcessConnectorCommand();

		if (comm->status != CommStatusReady) return False;

		Int64	 duration = Int64(comm->data[0]) << 32 | comm->data[1];

		/* Fix mhdr atom for long running tracks.
		 */
		if (duration > 0xFFFFFFFF)
		{
			String		 tempFile = String(track.outputFile).Append(".temp");

			InStream	 in(STREAM_FILE, track.outputFile, IS_READ);
			OutStream	 out(STREAM_FILE, tempFile, OS_REPLACE);

			Bool		 result = FixupDurationAtoms(duration, in, out, in.Size());

			in.Close();
			out.Close();

			if (result)
			{
				File(track.outputFile).Delete();
				File(tempFile).Move(track.outputFile);
			}

			File(tempFile).Delete();
		}

		/* Write metadata to file.
		 */
		const Info	&info = track.GetInfo();

		if (config->GetIntValue("Tags", "EnableMP4Metadata", True) && (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("mp4-tag");

			if (tagger != NIL)
			{
				tagger->SetConfiguration(config);
				tagger->RenderStreamInfo(track.outputFile, track);

				boca.DeleteComponent(tagger);
			}
		}
		else if (mp4v2dll != NIL)
		{
			/* Optimize file even when no tags are written.
			 */
			String	 tempFile = String(track.outputFile).Append(".temp");

			ex_MP4Optimize(track.outputFile.ConvertTo("UTF-8"), tempFile.ConvertTo("UTF-8"));

			File(track.outputFile).Delete();
			File(tempFile).Move(track.outputFile);
		}
	}

	/* Write ID3v1 tag if requested.
	 */
	if (fileType == CA::kAudioFileAAC_ADTSType && config->GetIntValue("Tags", "EnableID3v1", False))
	{
		const Info	&info = track.GetInfo();

		if (info.HasBasicInfo())
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v1-tag");

			if (tagger != NIL)
			{
				OutStream		 out(STREAM_FILE, track.outputFile, OS_APPEND);
				Buffer<unsigned char>	 id3Buffer;

				tagger->SetConfiguration(config);
				tagger->RenderBuffer(id3Buffer, track);

				out.OutputData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	/* Update ID3v2 tag with correct chapter marks.
	 */
	if (fileType == CA::kAudioFileAAC_ADTSType && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureCoreAudio::ConfigID, "AllowID3v2", False))
	{
		if (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				OutStream		 out(STREAM_FILE, track.outputFile, OS_APPEND);
				Buffer<unsigned char>	 id3Buffer;

				tagger->SetConfiguration(config);
				tagger->RenderBuffer(id3Buffer, track);

				out.Seek(0);
				out.OutputData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	return True;
}

Int BoCA::EncoderCoreAudioConnect::WriteData(Buffer<UnsignedByte> &data)
{
	const Format	&format	= track.GetFormat();

	/* Change to AAC channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::Default_3_0, Channel::AAC_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_0, Channel::AAC_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_1, Channel::AAC_5_1);
	else if (format.channels == 7) Utilities::ChangeChannelOrder(data, format, Channel::Default_6_1, Channel::AAC_6_1);
	else if (format.channels == 8) Utilities::ChangeChannelOrder(data, format, Channel::Default_7_1, Channel::AAC_7_1);

	/* Send Encode command.
	 */
	if (!connected) Connect();

	comm->command = CommCommandEncode;
	comm->length  = data.Size();

	memcpy(comm->data, data, data.Size());

	ProcessConnectorCommand();

	if (comm->status != CommStatusReady) return -1;
	else				     return data.Size();
}

Bool BoCA::EncoderCoreAudioConnect::SetOutputFormat(Int n)
{
	Config	*config = Config::Get();

	if (n != 1) config->SetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", True);
	else	    config->SetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", False);

	if	(n != 2 && config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC) == CA::kAudioFormatAppleLossless) config->SetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);
	else if (n == 2)															  config->SetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatAppleLossless);

	return True;
}

String BoCA::EncoderCoreAudioConnect::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	if (config->GetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", True))
	{
		switch (config->GetIntValue(ConfigureCoreAudio::ConfigID, "MP4FileExtension", 0))
		{
			default:
			case  0: return "m4a";
			case  1: return "m4b";
			case  2: return "m4r";
			case  3: return "mp4";
		}
	}

	return "aac";
}

Bool BoCA::EncoderCoreAudioConnect::FixupDurationAtoms(Int64 duration, InStream &in, OutStream &out, Int64 size, Bool seenMdat)
{
	Int64			 bytesLeft = size;
	Buffer<UnsignedByte>	 buffer(65536);

	while (bytesLeft > 0)
	{
		UnsignedInt64	 bytes	  = UnsignedInt32(in.InputNumberRaw(4));
		UnsignedInt32	 id	  = in.InputNumberRaw(4);
		Bool		 use64bit = (bytes == 1);
		Int		 grow	  = 0;

		if (use64bit == 1) bytes = in.InputNumberRaw(8);

		if	(id == 'moov') grow = 36;
		else if (id == 'trak') grow = 24;
		else if (id == 'mdia' ||
			 id == 'tkhd' ||
			 id == 'mdhd' ||
			 id == 'mvhd') grow = 12;

		out.OutputNumberRaw(use64bit ? 1 : bytes + grow, 4);
		out.OutputNumberRaw(id, 4);

		if (use64bit)
		{
			out.OutputNumberRaw(bytes + grow, 8);

			bytes	  -= 8;
			bytesLeft -= 8;
		}

		bytes	  -= 8;
		bytesLeft -= 8;

		/* Recurse into moov, trak, mdia, minf and stbl boxes.
		 */
		if (id == 'moov' ||
		    id == 'trak' ||
		    id == 'mdia' ||
		    id == 'minf' ||
		    id == 'stbl')
		{
			if (!FixupDurationAtoms(duration, in, out, bytes, seenMdat)) return False;

			bytesLeft -= bytes;

			continue;
		}

		/* Fixup duration atoms.
		 */
		if (id == 'tkhd' ||
		    id == 'mdhd' ||
		    id == 'mvhd')
		{
			Int	 size = 0;

			// Version.
			if (in.InputNumberRaw(1) >= 1) return False;

			out.OutputNumberRaw(1, 1);
			size += 1;

			// Flags.
			out.OutputNumberRaw(in.InputNumberRaw(3), 3);
			size += 3;

			// Creation time.
			out.OutputNumberRaw(0, 4);
			out.OutputNumberRaw(in.InputNumberRaw(4), 4);
			size += 4;

			// Modification time.
			out.OutputNumberRaw(0, 4);
			out.OutputNumberRaw(in.InputNumberRaw(4), 4);
			size += 4;

			// Time scale.
			out.OutputNumberRaw(in.InputNumberRaw(4), 4);
			size += 4;

			// Reserved.
			if (id == 'tkhd')
			{
				out.OutputNumberRaw(in.InputNumberRaw(4), 4);
				size += 4;
			}

			// Duration.
			in.RelSeek(4);
			out.OutputNumberRaw(duration, 8);
			size += 4;

			// Rest of atom.
			in.InputData(buffer, bytes - size);
			out.OutputData(buffer, bytes - size);

			bytesLeft -= bytes;

			continue;
		}

		/* Fixup chunk offset atoms.
		 */
		if (!seenMdat && (id == 'stco' ||
				  id == 'co64'))
		{
			// Version.
			out.OutputNumberRaw(in.InputNumberRaw(1), 1);

			// Flags.
			out.OutputNumberRaw(in.InputNumberRaw(3), 3);

			// Number of entries.
			UnsignedInt32	 numEntries = in.InputNumberRaw(4);

			out.OutputNumberRaw(numEntries, 4);

			// Entries.
			if (id == 'stco') for (UnsignedInt32 i = 0; i < numEntries; i++) out.OutputNumberRaw(in.InputNumberRaw(4) + 36, 4);
			else		  for (UnsignedInt32 i = 0; i < numEntries; i++) out.OutputNumberRaw(in.InputNumberRaw(8) + 36, 8);

			bytesLeft -= bytes;

			continue;
		}

		/* Copy other boxes/atoms.
		 */
		if (id == 'mdat') seenMdat = True;

		while (bytes > 0)
		{
			Int	 size = Math::Min(bytes, buffer.Size());

			in.InputData(buffer, size);
			out.OutputData(buffer, size);

			bytes	  -= size;
			bytesLeft -= size;
		}
	}

	return True;
}

Bool BoCA::EncoderCoreAudioConnect::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "coreaudio-enc";

	/* Set default values.
	 */
	if (!config->GetIntValue("Settings", "UserSpecifiedConfig", False))
	{
		config->SetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", True);

		config->SetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);
		config->SetIntValue(ConfigureCoreAudio::ConfigID, "Bitrate", 64);
	}

	/* Get command line settings.
	 */
	Bool	 rawAAC	    = config->GetIntValue(encoderID, "Write raw AAC files", !config->GetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", True));

	Int	 bitrate    = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Bitrate", 64);
	Int	 format	    = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);
	String	 formatName = "AAC";

	if (format == CA::kAudioFormatAppleLossless) formatName = "ALAC";

	if (config->GetIntValue(encoderID, "Set Bitrate per channel", False)) bitrate	 = config->GetIntValue(encoderID, "Bitrate per channel", bitrate);
	if (config->GetIntValue(encoderID, "Set Output format", False))	      formatName = config->GetStringValue(encoderID, "Output format", formatName).ToUpper();

	/* Set configuration values.
	 */
	config->SetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", !rawAAC || formatName == "ALAC");

	if	(formatName == "AAC" ) format = CA::kAudioFormatMPEG4AAC;
	else if (formatName == "ALAC") format = CA::kAudioFormatAppleLossless;

	config->SetIntValue(ConfigureCoreAudio::ConfigID, "Codec", format);
	config->SetIntValue(ConfigureCoreAudio::ConfigID, "Bitrate", Math::Max(8, Math::Min(256, bitrate)));

	return True;
}

ConfigLayer *BoCA::EncoderCoreAudioConnect::GetConfigurationLayer()
{
	if (configLayer == NIL)
	{
		static CoreAudioCommCodecs	 codecs;
		static Bool			 initialized = False;

		if (!initialized)
		{
			/* Send Codecs command.
			 */
			if (!connected) Connect();

			comm->command = CommCommandCodecs;
			comm->length  = sizeof(CoreAudioCommCodecs);

			ProcessConnectorCommand();

			if (comm->status == CommStatusReady) memcpy(&codecs, comm->data, comm->length);
			else				     codecs.codecs[0] = 0;

			initialized = True;
		}

		configLayer = new ConfigureCoreAudio(codecs);
	}

	return configLayer;
}

Bool BoCA::EncoderCoreAudioConnect::Connect()
{
	static multithread (intptr_t)	 count = 0;

	if (connected) return False;

	/* Try 32 and 64 bit connector.
	 */
	Array<String>	 files;

#ifdef __WIN32__
	files.Add(GUI::Application::GetApplicationDirectory().Append("boca\\boca_encoder_coreaudioconnect.1.0.exe"));
	files.Add(GUI::Application::GetApplicationDirectory().Append("boca\\boca_encoder_coreaudioconnect64.1.0.exe"));
#else
	files.Add(Utilities::GetBoCADirectory().Append("/boca_encoder_coreaudioconnect.1.0"));
	files.Add(Utilities::GetBoCADirectory().Append("/boca_encoder_coreaudioconnect64.1.0"));
#endif

	foreachreverse (const String &file, files) if (!File(file).Exists()) files.RemoveNth(foreachindex);

	foreach (const String &file, files)
	{
		/* Disconnect any previously established connections.
		 */
		Disconnect();

		/* Create shared memory object and map view to communication buffer.
		 */
#ifdef __WIN32__
		mappingName = BoCA::GetApplicationPrefix().Append(":").Append(Number((Int64) GetCurrentProcessId()).ToHexString()).Append("-").Append(Number((Int64) GetCurrentThreadId()).ToHexString()).Append("-").Append(Number((Int64) count++).ToHexString());

		semaphore   = CreateSemaphoreA(NIL, 0, 1, String(mappingName).Append("-sem"));
		mapping	    = CreateFileMappingA(INVALID_HANDLE_VALUE, NIL, PAGE_READWRITE, 0, sizeof(CoreAudioCommBuffer), mappingName);
		comm	    = (CoreAudioCommBuffer *) MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#else
		mappingName = String("/").Append(BoCA::GetApplicationPrefix()).Append(":").Append(Number((Int64) getpid()).ToHexString()).Append("-").Append(Number((Int64) pthread_self()).ToHexString()).Append("-").Append(Number((Int64) count++).ToHexString());

		key_t	 key = mappingName.ComputeCRC32();

		semaphore   = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
		mapping	    = shm_open(mappingName, O_CREAT | O_EXCL | O_RDWR, 0666);

		semctl(semaphore, 0, SETVAL, 0);
		ftruncate(mapping, sizeof(CoreAudioCommBuffer));

		comm	    = (CoreAudioCommBuffer *) mmap(NULL, sizeof(CoreAudioCommBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, mapping, 0);
#endif

		/* Start connector process.
		 */
#ifdef __WIN32__
		SHELLEXECUTEINFOA	 execInfo;

		ZeroMemory(&execInfo, sizeof(execInfo));

		execInfo.cbSize	      = sizeof(execInfo);
		execInfo.fMask	      = SEE_MASK_NOCLOSEPROCESS;
		execInfo.lpVerb	      = "open";
		execInfo.lpDirectory  = GUI::Application::GetApplicationDirectory();
		execInfo.nShow	      = SW_HIDE;
		execInfo.lpFile	      = file;
		execInfo.lpParameters = mappingName;

		ShellExecuteExA(&execInfo);

		connector = execInfo.hProcess;
#else
		const char	*cmd = String("wine ").Append(file).Append(" ").Append(mappingName).Append(" ").Append(String::FromInt(key)).Append(" 2> /dev/null");

		connector = fork();

		if (!connector) { execl("/bin/sh", "sh", "-c", cmd, NULL); exit(0); }
#endif

		connected = True;

		/* Send Hello command.
		 */
		comm->command = CommCommandHello;
		comm->length  = sizeof(CoreAudioCommHello);

		((CoreAudioCommHello *) &comm->data)->version = 1;

		ProcessConnectorCommand();

		if (comm->status == CommStatusReady) ready = True;
		else				     ready = False;

		if (ready) break;
	}

	return True;
}

Bool BoCA::EncoderCoreAudioConnect::Disconnect()
{
	if (!connected) return False;

	/* Send Quit command.
	 */
	comm->command = CommCommandQuit;
	comm->length  = 0;

	ProcessConnectorCommand();

	/* Wait until the connector exits.
	 */
#ifdef __WIN32__
	while (WaitForSingleObject(connector, 0) == WAIT_TIMEOUT) S::System::System::Sleep(1);
#else
	int	 status = 0;

	waitpid(connector, &status, 0);
#endif

	/* Unmap view and close shared memory object.
	 */
#ifdef __WIN32__
	UnmapViewOfFile(comm);
	CloseHandle(mapping);
	CloseHandle(semaphore);
#else
	munmap(comm, sizeof(CoreAudioCommBuffer));
	close(mapping);

	shm_unlink(mappingName);
	semctl(semaphore, 0, IPC_RMID, 0);
#endif

	connected = False;

	return True;
}

Bool BoCA::EncoderCoreAudioConnect::ProcessConnectorCommand()
{
	if (!connected) return False;

#ifndef __WIN32__
	static sembuf	 opWait[1] = { 0, -1, SEM_UNDO };
	static sembuf	 opPost[1] = { 0, 1,  SEM_UNDO };
#endif

	comm->status = CommStatusIssued;

	while (True)
	{
#ifdef __WIN32__
		ReleaseSemaphore(semaphore, 1, NULL);

		while (WaitForSingleObject(semaphore, 1000) != WAIT_OBJECT_0)
		{
			/* Check if the connector still exists.
			 */
			if (WaitForSingleObject(connector, 0) != WAIT_TIMEOUT) return False;
		}
#else
		semop(semaphore, opPost, 1);

		if (semop(semaphore, opWait, 1) != 0) return False;

		/* Check if the connector still exists.
		 */
		int	 status = 0;

		if (waitpid(connector, &status, WNOHANG) != 0) return False;
#endif

		if (comm->status != CommStatusIssued) break;
	}

	return True;
}
