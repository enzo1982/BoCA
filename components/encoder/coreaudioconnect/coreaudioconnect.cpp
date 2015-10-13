 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <windows.h>

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
		      <name>MP4 Audio Files</name>				\
		      <extension>m4a</extension>				\
		      <extension>m4b</extension>				\
		      <extension>m4r</extension>				\
		      <extension>mp4</extension>				\
		      <extension>3gp</extension>				\
		      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>	\
		    </format>							\
		    <format>							\
		      <name>Advanced Audio Files</name>				\
		      <extension>aac</extension>				\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>	\
		    </format>							\
		  </component>							\
										\
		";
	}

	return componentSpecs;
}

BoCA::EncoderCoreAudioConnect::EncoderCoreAudioConnect()
{
	configLayer = NIL;

	/* Create file mapping and map view to communication buffer.
	 */
	String	 mappingName = String("freac:").Append(Number((Int64) GetCurrentProcessId()).ToHexString()).Append("-").Append(Number((Int64) GetCurrentThreadId()).ToHexString());

	mapping	= CreateFileMappingA(INVALID_HANDLE_VALUE, NIL, PAGE_READWRITE, 0, sizeof(CoreAudioCommBuffer), mappingName);
	comm	= (CoreAudioCommBuffer *) MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	/* Start connector process.
	 */
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

	/* Send Hello command.
	 */
	comm->command = CommCommandHello;
	comm->length  = sizeof(CoreAudioCommHello);

	((CoreAudioCommHello *) &comm->data)->version = 1;

	ProcessConnectorCommand();

	if (comm->status == CommStatusReady) ready = True;
	else				     ready = False;
}

BoCA::EncoderCoreAudioConnect::~EncoderCoreAudioConnect()
{
	/* Send Quit command.
	 */
	comm->command = CommCommandQuit;
	comm->length  = 0;

	ProcessConnectorCommand();

	/* Unmap view and close file mapping.
	 */
	UnmapViewOfFile(comm);
	CloseHandle(mapping);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderCoreAudioConnect::IsReady() const
{
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

	if (format.channels > 2)
	{
		errorString = "This encoder does not support more than 2 channels!";
		errorState  = True;

		return False;
	}

	const Config	*config = GetConfiguration();

	/* Send Setup command.
	 */
	comm->command = CommCommandSetup;
	comm->length  = sizeof(CoreAudioCommSetup);

	((CoreAudioCommSetup *) &comm->data)->codec    = config->GetIntValue("CoreAudio", "Codec", 'aac ');
	((CoreAudioCommSetup *) &comm->data)->bitrate  = config->GetIntValue("CoreAudio", "Bitrate", 128) * 1000;
	((CoreAudioCommSetup *) &comm->data)->format   = config->GetIntValue("CoreAudio", "MP4Container", 1);

	((CoreAudioCommSetup *) &comm->data)->channels = format.channels;
	((CoreAudioCommSetup *) &comm->data)->rate     = format.rate;
	((CoreAudioCommSetup *) &comm->data)->bits     = format.bits;

	char	*outfile = Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out").ConvertTo("UTF-8");

	memcpy(((CoreAudioCommSetup *) &comm->data)->file, outfile, strlen(outfile) + 1);

	ProcessConnectorCommand();

	if (comm->status != CommStatusReady) return False;

	/* Write ID3v2 tag if requested.
	 */
	if (!config->GetIntValue("CoreAudio", "MP4Container", 1) && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue("CoreAudio", "AllowID3v2", 0))
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
	comm->command = CommCommandFinish;
	comm->length  = 0;

	ProcessConnectorCommand();

	if (comm->status != CommStatusReady) return False;

	/* Write metadata to file
	 */
	if (config->GetIntValue("CoreAudio", "MP4Container", 1) && config->GetIntValue("Tags", "EnableMP4Metadata", True))
	{
		const Info	&info = track.GetInfo();

		if (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("mp4-tag");

			if (tagger != NIL)
			{
				tagger->SetConfiguration(GetConfiguration());
				tagger->RenderStreamInfo(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), track);

				boca.DeleteComponent(tagger);
			}
		}
	}

	/* Stream contents of created MP4 file to output driver
	 */
	InStream		 in(STREAM_FILE, Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), IS_READ);
	Buffer<UnsignedByte>	 buffer(1024);
	Int64			 bytesLeft = in.Size();

	while (bytesLeft)
	{
		in.InputData(buffer, Math::Min(Int64(1024), bytesLeft));

		driver->WriteData(buffer, Math::Min(Int64(1024), bytesLeft));

		bytesLeft -= Math::Min(Int64(1024), bytesLeft);
	}

	in.Close();

	File(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out")).Delete();

	/* Write ID3v1 tag if requested.
	 */
	if (!config->GetIntValue("CoreAudio", "MP4Container", 1) && config->GetIntValue("Tags", "EnableID3v1", False))
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
	if (!config->GetIntValue("CoreAudio", "MP4Container", 1) && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue("CoreAudio", "AllowID3v2", 0))
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
	/* Send Encode command.
	 */
	comm->command = CommCommandEncode;
	comm->length  = data.Size();

	memcpy(comm->data, data, data.Size());

	ProcessConnectorCommand();

	if (comm->status != CommStatusReady) return -1;
	else				     return data.Size();
}

String BoCA::EncoderCoreAudioConnect::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	if (config->GetIntValue("CoreAudio", "MP4Container", 1))
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
		/* Send Codecs command.
		 */
		comm->command = CommCommandCodecs;
		comm->length  = sizeof(CoreAudioCommCodecs);

		ProcessConnectorCommand();

		if (comm->status == CommStatusReady)
		{
			CoreAudioCommCodecs	 codecs;

			memcpy(&codecs, comm->data, comm->length);

			configLayer = new ConfigureCoreAudio(codecs);
		}
	}

	return configLayer;
}

Bool BoCA::EncoderCoreAudioConnect::ProcessConnectorCommand()
{
	comm->status = CommStatusIssued;

	while (comm->status == CommStatusIssued || comm->status == CommStatusProcessing)
	{
		/* Check if the connector still exists.
		 */
		unsigned long	 exitCode = 0;

		GetExitCodeProcess(connector, &exitCode);

		if (exitCode != STILL_ACTIVE) break;

		/* Sleep for some milliseconds.
		 */
		if (comm->command == CommCommandEncode ||
		    comm->command == CommCommandFinish)	S::System::System::Sleep(0);
		else					S::System::System::Sleep(100);
	}

	return True;
}
