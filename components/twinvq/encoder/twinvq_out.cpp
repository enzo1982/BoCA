 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "twinvq_out.h"
#include "config.h"
#include "dllinterface.h"

#include "twinvq/bstream_e.h"
#include "twinvq/bstream_e.cxx"
#include "twinvq/Chunk.h"
#include "twinvq/Chunk.cxx"
#include "twinvq/ChunkHelper.h"
#include "twinvq/ChunkHelper.cxx"

const String &BoCA::TwinVQOut::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (twinvqdll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>TwinVQ VQF Encoder</name>		\
		    <version>1.0</version>			\
		    <id>twinvq-out</id>				\
		    <type>encoder</type>			\
		    <format>					\
		      <name>TwinVQ VQF Audio</name>		\
		      <extension>tvq</extension>		\
		    </format>					\
		  </component>					\
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

BoCA::TwinVQOut::TwinVQOut()
{
	configLayer = NIL;
}

BoCA::TwinVQOut::~TwinVQOut()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::TwinVQOut::Activate()
{
	Config	*config = Config::Get();

	switch (format.rate)
	{
		case 22050:
			if (config->GetIntValue("TwinVQ", "Bitrate", 48) == 48)
			{
				Utilities::ErrorMessage("Bad bitrate! The selected bitrate is not supported for this sampling rate.");

				errorState = True;

				return False;
			}
			break;
		case 44100:
			if (config->GetIntValue("TwinVQ", "Bitrate", 48) != 48)
			{
				Utilities::ErrorMessage("Bad bitrate! The selected bitrate is not supported for this sampling rate.");

				errorState = True;

				return False;
			}
			break;
		default:
			Utilities::ErrorMessage("Bad sampling rate! The selected sampling rate is not supported.");

			errorState = True;

			return False;
	}

	if (format.channels > 2)
	{
		Utilities::ErrorMessage("BonkEnc does not support more than 2 channels!");

		errorState = True;

		return False;
	}

	memset(&setupInfo, 0, sizeof(headerInfo));
	memset(&encInfo, 0, sizeof(encSpecificInfo));
	
	ex_TvqGetVersionID(V2, setupInfo.ID);

	setupInfo.channelMode = format.channels - 1;
	setupInfo.samplingRate = int(format.rate / 1000);
	setupInfo.bitRate = config->GetIntValue("TwinVQ", "Bitrate", 48) * format.channels;

	encInfo.N_CAN_GLOBAL = config->GetIntValue("TwinVQ", "PreselectionCandidates", 32); // number of VQ pre-selection candidates

	ex_TvqEncInitialize(&setupInfo, &encInfo, &index, 0);

	unsigned long	 samples_size = ex_TvqEncGetFrameSize() * ex_TvqEncGetNumChannels();

	packageSize = samples_size * (format.bits / 8);

	outBuffer.Resize(samples_size * (format.bits / 8));
	frame.Resize(samples_size);

	TvqInitBsWriter();

	CChunkChunk	*twinChunk	= TvqCreateHeaderChunk(&setupInfo, "header_info");
	OutStream	*d_out		= new OutStream(STREAM_BUFFER, outBuffer, outBuffer.Size());

	TvqPutBsHeaderInfo(d_out, *twinChunk);

	d_out->Flush();

	driver->WriteData(outBuffer, d_out->GetPos());

	delete twinChunk;
	delete d_out;

	return true;
}

Bool BoCA::TwinVQOut::Deactivate()
{
	OutStream	*d_out = new OutStream(STREAM_BUFFER, outBuffer, outBuffer.Size());

	frame.Zero();

	ex_TvqEncodeFrame(frame, &index);
	TvqWriteBsFrame(&index, d_out);

	ex_TvqEncodeFrame(frame, &index);
	TvqWriteBsFrame(&index, d_out);

	TvqFinishBsOutput(d_out);

	ex_TvqEncTerminate(&index);

	d_out->Flush();

	driver->WriteData(outBuffer, d_out->GetPos());

	delete d_out;

	return true;
}

Int BoCA::TwinVQOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	OutStream	*d_out = new OutStream(STREAM_BUFFER, outBuffer, outBuffer.Size());

	samplesBuffer.Resize(size / (format.bits / 8));

	for (int i = 0; i < size / (format.bits / 8); i++)
	{
		if (format.bits == 8)	samplesBuffer[i] = (data[i] - 128) * 256;
		if (format.bits == 16)	samplesBuffer[i] = ((short *) (unsigned char *) data)[i];
		if (format.bits == 24)	samplesBuffer[i] = (int) (data[3 * i] + 256 * data[3 * i + 1] + 65536 * data[3 * i + 2] - (data[3 * i + 2] & 128 ? 16777216 : 0)) / 256;
		if (format.bits == 32)	samplesBuffer[i] = (int) ((long *) (unsigned char *) data)[i] / 65536;
	}

	for (int ch = 0; ch < format.channels; ch++)
	{
		for (int i = 0; i < int(size / (format.bits / 8) / format.channels); i++)
		{
			frame[ch * int(frame.Size() / format.channels) + i] = (float) samplesBuffer[i * format.channels + ch];
		}
	}

	ex_TvqEncodeFrame(frame, &index);
	TvqWriteBsFrame(&index, d_out);

	size = d_out->GetPos();

	delete d_out;

	driver->WriteData(outBuffer, size);

	return size;
}

ConfigLayer *BoCA::TwinVQOut::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureTwinVQ();

	return configLayer;
}

Void BoCA::TwinVQOut::FreeConfigurationLayer()
{
	if (configLayer != NIL)
	{
		delete configLayer;

		configLayer = NIL;
	}
}
