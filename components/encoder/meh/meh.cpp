 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "meh.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderMultiEncoderHub::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>meh! - multi encoder hub</name>	\
	    <version>1.0</version>			\
	    <id>meh-enc</id>				\
	    <type>encoder</type>			\
	    <format>					\
	      <name>multi encoder hub Output</name>	\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::EncoderMultiEncoderHub::EncoderMultiEncoderHub()
{
	configLayer = NIL;
}

BoCA::EncoderMultiEncoderHub::~EncoderMultiEncoderHub()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);

	/* Delete empty folders if <filetype> was used in path.
	 */
	File(track.outfile).Delete();

	track.outfile[track.outfile.FindLast(Directory::GetDirectoryDelimiter())] = 0;

	while (track.outfile.Contains("[FILETYPE]"))
	{
		Directory(track.outfile).Delete();

		track.outfile[track.outfile.FindLast(Directory::GetDirectoryDelimiter())] = 0;
	}
}

Bool BoCA::EncoderMultiEncoderHub::IsThreadSafe() const
{
	const Config	*config = GetConfiguration();

	/* Check if at least one downstream encoder is not thread safe.
	 */
	AS::Registry		&boca	    = AS::Registry::Get();
	const Array<String>	&encoderIDs = config->GetStringValue("meh!", "Encoders", "flac-enc,lame-enc").Explode(",");
	Bool			 threadSafe = True;

	foreach (const String &encoderID, encoderIDs)
	{
		AS::EncoderComponent	*encoder = (AS::EncoderComponent *) boca.CreateComponentByID(encoderID);

		if (encoder != NIL)
		{
			encoder->SetConfiguration(config);

			if (!encoder->IsThreadSafe()) threadSafe = False;

			boca.DeleteComponent(encoder);
		}
	}

	String::ExplodeFinish();

	return threadSafe;
}

Bool BoCA::EncoderMultiEncoderHub::IsLossless() const
{
	const Config	*config = GetConfiguration();

	/* Check if at least one downstream encoder is lossless.
	 */
	AS::Registry		&boca	    = AS::Registry::Get();
	const Array<String>	&encoderIDs = config->GetStringValue("meh!", "Encoders", "flac-enc,lame-enc").Explode(",");
	Bool			 lossless   = False;

	foreach (const String &encoderID, encoderIDs)
	{
		AS::EncoderComponent	*encoder = (AS::EncoderComponent *) boca.CreateComponentByID(encoderID);

		if (encoder != NIL)
		{
			encoder->SetConfiguration(config);

			if (encoder->IsLossless()) lossless = True;

			boca.DeleteComponent(encoder);
		}
	}

	String::ExplodeFinish();

	return lossless;
}

Bool BoCA::EncoderMultiEncoderHub::Activate()
{
	const Config	*config = GetConfiguration();
	const Format	&format = track.GetFormat();

	/* Instantiate encoders.
	 */
	AS::Registry		&boca	    = AS::Registry::Get();
	const Array<String>	&encoderIDs = config->GetStringValue("meh!", "Encoders", "flac-enc,lame-enc").Explode(",");

	String	 fileNamePattern = track.outfile;

	if (fileNamePattern.EndsWith(".[FILETYPE]")) fileNamePattern[fileNamePattern.Length() - 11] = 0;

	if (config->GetIntValue("meh!", "SeparateFolders", False) && !config->GetIntValue("Settings", "EncodeToSingleFile", False))
	{
		String	 pre;
		String	 post = fileNamePattern;

		if (fileNamePattern.Contains(Directory::GetDirectoryDelimiter()))
		{
			pre  = fileNamePattern.Head(fileNamePattern.FindLast(Directory::GetDirectoryDelimiter()) + 1);
			post = fileNamePattern.Tail(fileNamePattern.Length() - fileNamePattern.FindLast(Directory::GetDirectoryDelimiter()) - 1);
		}

		fileNamePattern = String(pre).Append("[FILETYPE]").Append(Directory::GetDirectoryDelimiter()).Append(post);
	}

	foreach (const String &encoderID, encoderIDs)
	{
		AS::EncoderComponent	*encoder = (AS::EncoderComponent *) boca.CreateComponentByID(encoderID);

		if (encoder != NIL)
		{
			/* Set up encoder and stream.
			 */
			Track	 encoderTrack = track;

			encoderTrack.outfile.Append(".").Append(encoder->GetOutputFileExtension());

			encoder->SetConfiguration(config);
			encoder->SetAudioTrackInfo(encoderTrack);

			String		 fileName = String(fileNamePattern).Replace("[FILETYPE]", encoder->GetOutputFileExtension().ToUpper()).Append(".").Append(encoder->GetOutputFileExtension());
			OutStream	*stream	  = new OutStream(STREAM_FILE, Utilities::CreateDirectoryForFile(fileName), OS_REPLACE);

			stream->SetPackageSize(32768 * format.channels * (format.bits / 8));
			stream->SetFilter(encoder);

			encoders.Add(encoder);
			streams.Add(stream);
		}
	}

	String::ExplodeFinish();

	return True;
}

Bool BoCA::EncoderMultiEncoderHub::Deactivate()
{
	/* Free encoders.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	for (Int i = encoders.Length() - 1; i >= 0; i--)
	{
		/* Finish and delete encoder and stream.
		 */
		AS::EncoderComponent	*encoder = encoders.GetNth(i);
		OutStream		*stream	 = streams.GetNth(i);

		Track	 encoderTrack = track;

		encoderTrack.outfile.Append(".").Append(encoder->GetOutputFileExtension());

		encoder->SetAudioTrackInfo(encoderTrack);

		stream->RemoveFilter();

		if (encoder->GetErrorState()) { errorState = True; errorString = encoder->GetErrorString(); }

		boca.DeleteComponent(encoder);

		delete stream;
	}

	encoders.RemoveAll();
	streams.RemoveAll();

	return True;
}

Int BoCA::EncoderMultiEncoderHub::WriteData(Buffer<UnsignedByte> &data)
{
	/* Hand data to encoders.
	 */
	foreach (OutStream *stream, streams) stream->OutputData(data, data.Size());

	return data.Size();
}

String BoCA::EncoderMultiEncoderHub::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	if (!config->GetIntValue("Settings", "EncodeToSingleFile", False)) return "[FILETYPE]";

	return NIL;
}

ConfigLayer *BoCA::EncoderMultiEncoderHub::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureMultiEncoderHub();

	return configLayer;
}
