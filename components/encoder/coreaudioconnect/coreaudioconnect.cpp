 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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

#ifdef __WIN32__
#	include <windows.h>
#else
#	include <sys/mman.h>
#	include <sys/wait.h>
#	include <stdlib.h>
#	include <unistd.h>
#	include <fcntl.h>
#	include <pthread.h>
#endif

#include "coreaudioconnect.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderCoreAudioConnect::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (EncoderCoreAudioConnect().IsReady())
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>Core Audio AAC/ALAC Encoder</name>			\
		    <version>1.0</version>					\
		    <id>coreaudio-enc</id>					\
		    <type>encoder</type>					\
		    <replace>faac-enc</replace>					\
		    <replace>voaacenc-enc</replace>				\
		    <replace>avconv-alac-enc</replace>				\
		    <format>							\
		      <name>MPEG-4 AAC Files</name>				\
		      <extension>m4a</extension>				\
		      <extension>m4b</extension>				\
		      <extension>m4r</extension>				\
		      <extension>mp4</extension>				\
		      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>	\
		    </format>							\
		    <format>							\
		      <name>Raw AAC Files</name>				\
		      <extension>aac</extension>				\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>	\
		    </format>							\
		    <format>							\
		      <name>Apple Lossless Files</name>				\
		      <extension>m4a</extension>				\
		      <extension>m4b</extension>				\
		      <extension>mp4</extension>				\
		      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>	\
		    </format>							\
		  </component>							\
										\
		";
	}

	return componentSpecs;
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

	configLayer = NIL;
}

BoCA::EncoderCoreAudioConnect::~EncoderCoreAudioConnect()
{
	if (connected) Disconnect();

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderCoreAudioConnect::IsReady() const
{
	if (!connected) const_cast<EncoderCoreAudioConnect *>(this)->Connect();

	return ready;
}

Bool BoCA::EncoderCoreAudioConnect::IsLossless() const
{
	const Config	*config = GetConfiguration();

	if (config->GetIntValue("CoreAudio", "Codec", 'aac ') == 'alac') return True;

	return False;
}

Bool BoCA::EncoderCoreAudioConnect::Activate()
{
	const Format	&format = track.GetFormat();

	if (format.channels > 8)
	{
		errorString = "This encoder does not support more than 8 channels!";
		errorState  = True;

		return False;
	}

	const Config	*config = GetConfiguration();

	/* Send Setup command.
	 */
	if (!connected) Connect();

	comm->command = CommCommandSetup;
	comm->length  = sizeof(CoreAudioCommSetup);

	((CoreAudioCommSetup *) &comm->data)->codec    = config->GetIntValue("CoreAudio", "Codec", 'aac ');
	((CoreAudioCommSetup *) &comm->data)->bitrate  = config->GetIntValue("CoreAudio", "Bitrate", 128) * 1000;
	((CoreAudioCommSetup *) &comm->data)->format   = config->GetIntValue("CoreAudio", "MP4Container", True);

	((CoreAudioCommSetup *) &comm->data)->channels = format.channels;
	((CoreAudioCommSetup *) &comm->data)->rate     = format.rate;
	((CoreAudioCommSetup *) &comm->data)->bits     = format.bits;

	fileName = Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out");

#ifndef __WIN32__
	fileName = fileName.Tail(fileName.Length() - fileName.FindLast(Directory::GetDirectoryDelimiter()) - 1);
#endif

	char	*outfile = fileName.ConvertTo("UTF-8");

	memcpy(((CoreAudioCommSetup *) &comm->data)->file, outfile, strlen(outfile) + 1);

	ProcessConnectorCommand();

	if (comm->status != CommStatusReady) return False;

	fileName.ImportFrom("UTF-8", ((CoreAudioCommSetup *) &comm->data)->file);

	/* Write ID3v2 tag if requested.
	 */
	if (!config->GetIntValue("CoreAudio", "MP4Container", True) && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue("CoreAudio", "AllowID3v2", False))
	{
		const Info	&info = track.GetInfo();

		if (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->SetConfiguration(GetConfiguration());
				tagger->RenderBuffer(id3Buffer, track);

				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	return True;
}

Bool BoCA::EncoderCoreAudioConnect::Deactivate()
{
	const Config	*config = GetConfiguration();

	/* Send Finish command.
	 */
	if (!connected) Connect();

	comm->command = CommCommandFinish;
	comm->length  = 0;

	ProcessConnectorCommand();

	if (comm->status != CommStatusReady) return False;

	/* Write metadata to file
	 */
	if (config->GetIntValue("CoreAudio", "MP4Container", True) && config->GetIntValue("Tags", "EnableMP4Metadata", True))
	{
		const Info	&info = track.GetInfo();

		if (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("mp4-tag");

			if (tagger != NIL)
			{
				tagger->SetConfiguration(GetConfiguration());
				tagger->RenderStreamInfo(fileName, track);

				boca.DeleteComponent(tagger);
			}
		}
	}

	/* Stream contents of created MP4 file to output driver
	 */
	InStream		 in(STREAM_FILE, fileName, IS_READ);
	Buffer<UnsignedByte>	 buffer(1024);
	Int64			 bytesLeft = in.Size();

	while (bytesLeft)
	{
		in.InputData(buffer, Math::Min(Int64(1024), bytesLeft));

		driver->WriteData(buffer, Math::Min(Int64(1024), bytesLeft));

		bytesLeft -= Math::Min(Int64(1024), bytesLeft);
	}

	in.Close();

	File(fileName).Delete();

	/* Write ID3v1 tag if requested.
	 */
	if (!config->GetIntValue("CoreAudio", "MP4Container", True) && config->GetIntValue("Tags", "EnableID3v1", False))
	{
		const Info	&info = track.GetInfo();

		if (info.HasBasicInfo())
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v1-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->SetConfiguration(GetConfiguration());
				tagger->RenderBuffer(id3Buffer, track);

				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	/* Update ID3v2 tag with correct chapter marks.
	 */
	if (!config->GetIntValue("CoreAudio", "MP4Container", True) && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue("CoreAudio", "AllowID3v2", False))
	{
		if (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->SetConfiguration(GetConfiguration());
				tagger->RenderBuffer(id3Buffer, track);

				driver->Seek(0);
				driver->WriteData(id3Buffer, id3Buffer.Size());

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

	if (n != 1) config->SetIntValue("CoreAudio", "MP4Container", True);
	else	    config->SetIntValue("CoreAudio", "MP4Container", False);

	if	(n != 2 && config->GetIntValue("CoreAudio", "Codec", 'aac ') == 'alac') config->SetIntValue("CoreAudio", "Codec", 'aac ');
	else if (n == 2)								config->SetIntValue("CoreAudio", "Codec", 'alac');

	return True;
}

String BoCA::EncoderCoreAudioConnect::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	if (config->GetIntValue("CoreAudio", "MP4Container", True))
	{
		switch (config->GetIntValue("CoreAudio", "MP4FileExtension", 0))
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
	static multithread (int)	 count = 0;

	if (connected) return False;

	/* Create shared memory object and map view to communication buffer.
	 */
#ifdef __WIN32__
	mappingName = String("freac:").Append(Number((Int64) GetCurrentProcessId()).ToHexString()).Append("-").Append(Number((Int64) GetCurrentThreadId()).ToHexString()).Append("-").Append(Number((Int64) count++).ToHexString());

	mapping	    = CreateFileMappingA(INVALID_HANDLE_VALUE, NIL, PAGE_READWRITE, 0, sizeof(CoreAudioCommBuffer), mappingName);
	comm	    = (CoreAudioCommBuffer *) MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#else
	mappingName = String("/freac:").Append(Number((Int64) getpid()).ToHexString()).Append("-").Append(Number((Int64) pthread_self()).ToHexString()).Append("-").Append(Number((Int64) count++).ToHexString());

	mapping	    = shm_open(mappingName, O_CREAT | O_EXCL | O_RDWR, 0666);

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
	execInfo.lpFile	      = GUI::Application::GetApplicationDirectory().Append("boca\\boca_encoder_coreaudioconnect.1.0.exe");
	execInfo.lpParameters = mappingName;

	ShellExecuteExA(&execInfo);

	connector = execInfo.hProcess;
#else
	const char	*cmd = String("wine ").Append(Utilities::GetBoCADirectory()).Append("/boca_encoder_coreaudioconnect.1.0 ").Append(mappingName).Append(" 2> /dev/null");

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
#else
	munmap(comm, sizeof(CoreAudioCommBuffer));
	close(mapping);

	shm_unlink(mappingName);
#endif

	return True;
}

Bool BoCA::EncoderCoreAudioConnect::ProcessConnectorCommand()
{
	if (!connected) return False;

	comm->status = CommStatusIssued;

	while (comm->status == CommStatusIssued || comm->status == CommStatusProcessing)
	{
		/* Check if the connector still exists.
		 */
#ifdef __WIN32__
		if (WaitForSingleObject(connector, 0) != WAIT_TIMEOUT) break;
#else
		int	 status = 0;

		if (waitpid(connector, &status, WNOHANG) != 0) break;
#endif

		/* Sleep for one millisecond.
		 */
		S::System::System::Sleep(1);
	}

	return True;
}
