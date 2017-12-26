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
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>TwinVQ VQF Encoder</name>				\
		    <version>1.0</version>					\
		    <id>twinvq-enc</id>						\
		    <type threadSafe=\"false\">encoder</type>			\
		    <format>							\
		      <name>TwinVQ VQF Audio</name>				\
		      <extension>vqf</extension>				\
		    </format>							\
		    <input bits=\"16\" channels=\"1-2\" rate=\"22050,44100\"/>	\
		  </component>							\
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

	memset(&setupInfo, 0, sizeof(setupInfo));
	memset(&encInfo, 0, sizeof(encInfo));
	memset(&index, 0, sizeof(index));
}

BoCA::EncoderTwinVQ::~EncoderTwinVQ()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderTwinVQ::Activate()
{
	const Config	*config = GetConfiguration();

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
		const Config	*currentConfig = GetConfiguration();

		if	(info.artist != NIL) strncpy(setupInfo.Auth, info.artist, Math::Min(info.artist.Length() + 1, sizeof(setupInfo.Auth)));
		if	(info.title  != NIL) strncpy(setupInfo.Name, info.title,  Math::Min(info.title.Length() + 1,  sizeof(setupInfo.Name)));

		if	(info.comment != NIL && !config->GetIntValue("Tags", "ReplaceExistingComments", False)) strncpy(setupInfo.Comt, info.comment,						      Math::Min(info.comment.Length() + 1,						   sizeof(setupInfo.Comt)));
		else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL)			strncpy(setupInfo.Comt, currentConfig->GetStringValue("Tags", "DefaultComment", NIL), Math::Min(currentConfig->GetStringValue("Tags", "DefaultComment", NIL).Length() + 1, sizeof(setupInfo.Comt)));
	}

	encInfo.N_CAN_GLOBAL = config->GetIntValue(ConfigureTwinVQ::ConfigID, "PreselectionCandidates", 32); // number of VQ pre-selection candidates

	ex_TvqEncInitialize(&setupInfo, &encInfo, &index, 0);

	unsigned long	 samples_size = ex_TvqEncGetFrameSize() * ex_TvqEncGetNumChannels();

	packageSize = samples_size * (format.bits / 8);

	frame.Resize(samples_size);

	TvqInitBsWriter();

	CChunkChunk	*twinChunk	= TvqCreateHeaderChunk(&setupInfo, "header_info");

	bfp = bopen(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), (char *) "wb");

	TvqPutBsHeaderInfo(bfp, *twinChunk);

	delete twinChunk;

	return True;
}

Bool BoCA::EncoderTwinVQ::Deactivate()
{
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
	/* Output samples to encoder.
	 */
	const Format	&format = track.GetFormat();

	for (Int ch = 0; ch < format.channels; ch++)
	{
		for (Int i = 0; i < int(data.Size() / sizeof(short) / format.channels); i++)
		{
			frame[ch * int(frame.Size() / format.channels) + i] = (float) ((short *) (UnsignedByte *) data)[i * format.channels + ch];
		}
	}

	ex_TvqEncodeFrame(frame, &index);
	TvqWriteBsFrame(&index, bfp);

	return data.Size();
}

ConfigLayer *BoCA::EncoderTwinVQ::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureTwinVQ();

	return configLayer;
}
