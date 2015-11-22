 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
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
	finished    = False;
	cancelled   = False;

	configLayer = NIL;

	Engine::Get()->onCancelTrackConversion.Connect(&EncoderMultiEncoderHub::OnCancelTrackConversion, this);
}

BoCA::EncoderMultiEncoderHub::~EncoderMultiEncoderHub()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);

	Engine::Get()->onCancelTrackConversion.Disconnect(&EncoderMultiEncoderHub::OnCancelTrackConversion, this);

	/* Delete output file if it still exists.
	 */
	if (track.outfile != NIL)
	{
		File(track.outfile).Delete();

		if (track.outfile.Contains(Directory::GetDirectoryDelimiter())) track.outfile[track.outfile.FindLast(Directory::GetDirectoryDelimiter())] = 0;
	}

	/* Delete empty folders if <filetype> was used in path.
	 */
	while (track.outfile.Contains("[FILETYPE]"))
	{
		Directory(track.outfile).Delete();

		if (track.outfile.Contains(Directory::GetDirectoryDelimiter())) track.outfile[track.outfile.FindLast(Directory::GetDirectoryDelimiter())] = 0;
		else								break;
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

	finished  = False;
	cancelled = False;

	/* Instantiate encoders.
	 */
	AS::Registry		&boca	    = AS::Registry::Get();
	const Array<String>	&encoderIDs = config->GetStringValue("meh!", "Encoders", "flac-enc,lame-enc").Explode(",");

	String	 fileNamePattern = GetFileNamePattern();

	foreach (const String &encoderID, encoderIDs)
	{
		AS::EncoderComponent	*encoder = (AS::EncoderComponent *) boca.CreateComponentByID(encoderID);

		if (encoder != NIL)
		{
			/* Set up encoder and stream.
			 */
			encoder->SetConfiguration(config);

			Track	 encoderTrack = track;
			String	 fileName     = String(fileNamePattern).Replace("[FILETYPE]", encoder->GetOutputFileExtension().ToUpper()).Append(".").Append(encoder->GetOutputFileExtension());

			encoderTrack.outfile = fileName;

			encoder->SetAudioTrackInfo(encoderTrack);

			OutStream	*stream = new OutStream(STREAM_FILE, Utilities::CreateDirectoryForFile(fileName), OS_REPLACE);

			stream->SetPackageSize(32768 * format.channels * (format.bits / 8));
			stream->SetFilter(encoder);

			encoders.Add(encoder);
			streams.Add(stream);

			/* Set up mutexes, buffers and threads.
			 */
			if ((config->GetIntValue("Settings", "EncodeToSingleFile", False) || !IsThreadSafe()) && CPU().GetNumLogicalCPUs() > 1)
			{
				mutexes.Add(new Threads::Mutex());
				buffers.Add(new Buffer<UnsignedByte>);
				threads.Add(NonBlocking1<Int>(&EncoderMultiEncoderHub::EncodeThread, this).Call(threads.Length()));
			}
		}
	}

	String::ExplodeFinish();

	return True;
}

Bool BoCA::EncoderMultiEncoderHub::Deactivate()
{
	const Config	*config = GetConfiguration();

	/* Signal encoder threads that we are done.
	 */
	finished = True;

	/* Free encoders.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	String	 fileNamePattern = GetFileNamePattern();

	for (Int i = encoders.Length() - 1; i >= 0; i--)
	{
		/* Delete mutexes, buffers and threads.
		 */
		if (threads.Length() > 0)
		{
			threads.GetNth(i)->Wait();

			delete threads.GetNth(i);
			delete buffers.GetNth(i);
			delete mutexes.GetNth(i);
		}

		/* Finish and delete encoder and stream.
		 */
		AS::EncoderComponent	*encoder = encoders.GetNth(i);
		OutStream		*stream	 = streams.GetNth(i);

		Track	 encoderTrack = track;
		String	 fileName     = String(fileNamePattern).Replace("[FILETYPE]", encoder->GetOutputFileExtension().ToUpper()).Append(".").Append(encoder->GetOutputFileExtension());

		encoderTrack.outfile = fileName;

		encoder->SetAudioTrackInfo(encoderTrack);

		stream->RemoveFilter();

		if (encoder->GetErrorState()) { errorState = True; errorString = encoder->GetErrorString(); }

		delete stream;

		boca.DeleteComponent(encoder);

		if (cancelled)
		{
			File(encoderTrack.outfile).Delete();

			if (config->GetIntValue("meh!", "SeparateFolders", False) && !config->GetIntValue("Settings", "EncodeToSingleFile", False))
			{
				encoderTrack.outfile[encoderTrack.outfile.FindLast(Directory::GetDirectoryDelimiter())] = 0;

				Directory(encoderTrack.outfile).Delete();
			}
		}
	}

	encoders.RemoveAll();
	streams.RemoveAll();

	mutexes.RemoveAll();
	buffers.RemoveAll();
	threads.RemoveAll();

	return True;
}

Int BoCA::EncoderMultiEncoderHub::WriteData(Buffer<UnsignedByte> &data)
{
	/* Hand data to encoders.
	 */
	if (threads.Length() == 0)
	{
		foreach (OutStream *stream, streams) stream->OutputData(data, data.Size());
	}
	else
	{
		for (Int i = 0; i < threads.Length(); i++)
		{
			Threads::Mutex		*mutex	= mutexes.GetNth(i);
			Buffer<UnsignedByte>	*buffer = buffers.GetNth(i);

			mutex->Lock();

			buffer->Resize(data.Size());

			memcpy(*buffer, data, data.Size());

			mutex->Release();
		}
	}

	return data.Size();
}

String BoCA::EncoderMultiEncoderHub::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	if (!config->GetIntValue("Settings", "EncodeToSingleFile", False)) return "[FILETYPE]";

	return NIL;
}

String BoCA::EncoderMultiEncoderHub::GetFileNamePattern() const
{
	const Config	*config = GetConfiguration();

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

	return fileNamePattern;
}

Void BoCA::EncoderMultiEncoderHub::EncodeThread(Int n)
{
	Threads::Mutex		*mutex	= mutexes.GetNth(n);
	Buffer<UnsignedByte>	*buffer = buffers.GetNth(n);

	OutStream		*stream = streams.GetNth(n);

	while (!finished)
	{
		while (buffer->Size() == 0 && !finished) S::System::System::Sleep(0);

		mutex->Lock();

		stream->OutputData(*buffer, buffer->Size());

		buffer->Resize(0);

		mutex->Release();
	}
}

Void BoCA::EncoderMultiEncoderHub::OnCancelTrackConversion(const Track &cancelledTrack)
{
	const Config	*config = GetConfiguration();

	if ((config->GetIntValue("Settings", "EncodeToSingleFile", False) && cancelledTrack.outfile == track.outfile) ||
									     cancelledTrack.GetTrackID() == track.GetTrackID()) cancelled = True;
}

ConfigLayer *BoCA::EncoderMultiEncoderHub::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureMultiEncoderHub();

	return configLayer;
}
