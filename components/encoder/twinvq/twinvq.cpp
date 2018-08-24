 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
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

using namespace smooth::IO;

#include "twinvq.h"
#include "config.h"

#include "twinvq/bfile_e.h"
#include "twinvq/bstream_e.h"
#include "twinvq/Chunk.h"
#include "twinvq/ChunkHelper.h"

const String &BoCA::EncoderTwinVQ::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (twinvqdll != NIL)
	{
		componentSpecs = "										\
														\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>							\
		  <component>											\
		    <name>TwinVQ VQF Encoder</name>								\
		    <version>1.0</version>									\
		    <id>twinvq-enc</id>										\
		    <type threadSafe=\"false\">encoder</type>							\
		    <format>											\
		      <name>TwinVQ VQF Audio</name>								\
		      <extension>vqf</extension>								\
		    </format>											\
		    <input bits=\"16\" channels=\"1-2\" rate=\"22050,44100\"/>					\
		    <parameters>										\
		      <selection name=\"Bitrate per channel\" argument=\"-b %VALUE\" default=\"48\">		\
			<option alias=\"24 kbps\">24</option>							\
			<option alias=\"32 kbps\">32</option>							\
			<option alias=\"48 kbps\">48</option>							\
		      </selection>										\
		      <selection name=\"Preselection candidates\" argument=\"-c %VALUE\" default=\"32\">	\
			<option>4</option>									\
			<option>8</option>									\
			<option>16</option>									\
			<option>32</option>									\
		      </selection>										\
		    </parameters>										\
		  </component>											\
														\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadTwinVQDLL();
}

Void smooth::DetachDLL()
{
	FreeTwinVQDLL();
}

BoCA::EncoderTwinVQ::EncoderTwinVQ()
{
	configLayer = NIL;

	bfp	    = NIL;

	config	    = Config::Copy(GetConfiguration());

	ConvertArguments(config);

	memset(&setupInfo, 0, sizeof(setupInfo));
	memset(&encInfo, 0, sizeof(encInfo));
	memset(&index, 0, sizeof(index));
}

BoCA::EncoderTwinVQ::~EncoderTwinVQ()
{
	Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderTwinVQ::Activate()
{
	const Format	&format = track.GetFormat();
	const Info	&info	= track.GetInfo();

	/* Check settings.
	 */
	switch (format.rate)
	{
		case 22050:
			if (config->GetIntValue(ConfigureTwinVQ::ConfigID, "Bitrate", 48) == 48)
			{
				errorString = "Bad bitrate! The selected bitrate is not supported for this sampling rate.";
				errorState  = True;

				return False;
			}
			break;
		case 44100:
			if (config->GetIntValue(ConfigureTwinVQ::ConfigID, "Bitrate", 48) != 48)
			{
				errorString = "Bad bitrate! The selected bitrate is not supported for this sampling rate.";
				errorState  = True;

				return False;
			}
			break;
	}

	/* Create and configure TwinVQ encoder.
	 */
	memset(&setupInfo, 0, sizeof(headerInfo));
	memset(&encInfo, 0, sizeof(encSpecificInfo));

	ex_TvqGetVersionID(V2, setupInfo.ID);

	setupInfo.channelMode  = format.channels - 1;
	setupInfo.samplingRate = int(format.rate / 1000);
	setupInfo.bitRate      = config->GetIntValue(ConfigureTwinVQ::ConfigID, "Bitrate", 48) * format.channels;

	if (info.HasBasicInfo())
	{
		if	(info.artist != NIL) strncpy(setupInfo.Auth, info.artist, Math::Min(info.artist.Length() + 1, sizeof(setupInfo.Auth)));
		if	(info.title  != NIL) strncpy(setupInfo.Name, info.title,  Math::Min(info.title.Length() + 1,  sizeof(setupInfo.Name)));

		if	(info.comment != NIL && !config->GetIntValue("Tags", "ReplaceExistingComments", False)) strncpy(setupInfo.Comt, info.comment,					       Math::Min(info.comment.Length() + 1,					     sizeof(setupInfo.Comt)));
		else if (config->GetStringValue("Tags", "DefaultComment", NIL) != NIL)				strncpy(setupInfo.Comt, config->GetStringValue("Tags", "DefaultComment", NIL), Math::Min(config->GetStringValue("Tags", "DefaultComment", NIL).Length() + 1, sizeof(setupInfo.Comt)));
	}

	encInfo.N_CAN_GLOBAL = config->GetIntValue(ConfigureTwinVQ::ConfigID, "PreselectionCandidates", 32); // number of VQ pre-selection candidates

	ex_TvqEncInitialize(&setupInfo, &encInfo, &index, 0);

	frame.Resize(ex_TvqEncGetFrameSize() * ex_TvqEncGetNumChannels());

	TvqInitBsWriter();

	CChunkChunk	*twinChunk	= TvqCreateHeaderChunk(&setupInfo, "header_info");

	bfp = bopen(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), (char *) "wb");

	TvqPutBsHeaderInfo(bfp, *twinChunk);

	delete twinChunk;

	return True;
}

Bool BoCA::EncoderTwinVQ::Deactivate()
{
	const Format	&format = track.GetFormat();

	/* Output remaining samples.
	 */
	frame.Zero();

	for (Int ch = 0; ch < format.channels; ch++)
	{
		for (Int i = 0; i < samplesBuffer.Size() / format.channels; i++)
		{
			frame[ch * frame.Size() / format.channels + i] = (float) ((short *) samplesBuffer)[i * format.channels + ch];
		}
	}

	ex_TvqEncodeFrame(frame, &index);
	TvqWriteBsFrame(&index, bfp);

	/* Flush TwinVQ buffers by writing two empty frames.
	 */
	frame.Zero();

	ex_TvqEncodeFrame(frame, &index);
	TvqWriteBsFrame(&index, bfp);

	ex_TvqEncodeFrame(frame, &index);
	TvqWriteBsFrame(&index, bfp);

	ex_TvqEncTerminate(&index);

	bclose(bfp);

	/* Stream contents of created VQF file to output driver
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

	return True;
}

Int BoCA::EncoderTwinVQ::WriteData(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	/* Copy data to samples buffer.
	 */
	Int	 samples = data.Size() / 2;

	samplesBuffer.Resize(samplesBuffer.Size() + samples);

	memcpy(samplesBuffer + samplesBuffer.Size() - samples, data, data.Size());

	/* Output samples to encoder.
	 */
	Int	 framesProcessed = 0;

	while (samplesBuffer.Size() - framesProcessed * frame.Size() >= frame.Size())
	{
		for (Int ch = 0; ch < format.channels; ch++)
		{
			for (Int i = 0; i < frame.Size() / format.channels; i++)
			{
				frame[ch * frame.Size() / format.channels + i] = (float) ((short *) samplesBuffer + framesProcessed * frame.Size())[i * format.channels + ch];
			}
		}

		ex_TvqEncodeFrame(frame, &index);
		TvqWriteBsFrame(&index, bfp);

		framesProcessed++;
	}

	memmove(samplesBuffer, samplesBuffer + framesProcessed * frame.Size(), sizeof(short) * (samplesBuffer.Size() - framesProcessed * frame.Size()));

	samplesBuffer.Resize(samplesBuffer.Size() - framesProcessed * frame.Size());

	return data.Size();
}

Bool BoCA::EncoderTwinVQ::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "twinvq-enc";

	/* Get command line settings.
	 */
	Int	 bitrate    = 48;
	Int	 candidates = 32;

	if (config->GetIntValue(encoderID, "Set Bitrate per channel", False))     bitrate    = config->GetIntValue(encoderID, "Bitrate per channel", bitrate);
	if (config->GetIntValue(encoderID, "Set Preselection candidates", False)) candidates = config->GetIntValue(encoderID, "Preselection candidates", candidates);

	/* Set configuration values.
	 */
	config->SetIntValue(ConfigureTwinVQ::ConfigID, "Bitrate", Math::Max(24, Math::Min(48, bitrate)));
	config->SetIntValue(ConfigureTwinVQ::ConfigID, "PreselectionCandidates", Math::Max(4, Math::Min(32, candidates)));

	return True;
}

ConfigLayer *BoCA::EncoderTwinVQ::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureTwinVQ();

	return configLayer;
}
