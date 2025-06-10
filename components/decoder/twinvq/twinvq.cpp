 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2025 Robert Kausch <robert.kausch@freac.org>
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

#include <iostream>

#include "twinvq.h"

#include "twinvq/bfile.h"
#include "twinvq/bstream.h"
#include "twinvq/HeaderManager.h"

const String &BoCA::DecoderTwinVQ::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (twinvqdll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>TwinVQ VQF Decoder</name>		\
		    <version>1.0</version>			\
		    <id>twinvq-dec</id>				\
		    <type threadSafe=\"false\">decoder</type>	\
		    <replace>ffmpeg-vqf-dec</replace>		\
		    <format>					\
		      <name>TwinVQ VQF Audio</name>		\
		      <extension>vqf</extension>		\
		    </format>					\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

static int GetTextInfo(char *data, std::string ID, CHeaderManager *theManager)
{
	try
	{
		CStringChunk stringChunk = theManager->GetPrimaryChunk(ID);

		sprintf(data, "%s", stringChunk.GetString().c_str());
	}
	catch (CHeaderManager::err_FailGetChunk)
	{
		strncpy(data, "", 1);

		return 1;
	}

	return 0;
}

static int TvqGetStandardChunkInfo(CHeaderManager *theManager, headerInfo *setupInfo)
{
	memset(setupInfo, 0, sizeof(headerInfo));

	// get TwinVQ ID
	sprintf(setupInfo->ID, "%s", theManager->GetID().c_str());

	// get COMM chunk information
	try
	{
		CCommChunk commChunk(theManager->GetPrimaryChunk("COMM"));

		setupInfo->channelMode	 = commChunk.GetChannelMode();
		setupInfo->bitRate	 = commChunk.GetBitRate();
		setupInfo->samplingRate	 = commChunk.GetSamplingRate();
		setupInfo->securityLevel = commChunk.GetSecurityLevel();
	}
	catch (CHeaderManager::err_FailGetChunk)
	{
		std::cerr << "no COMM chunk" << std::endl;

		return 1;
	}

	GetTextInfo(setupInfo->Name, "NAME", theManager);
	GetTextInfo(setupInfo->Auth, "AUTH", theManager);
	GetTextInfo(setupInfo->Comt, "COMT", theManager);
	GetTextInfo(setupInfo->Cpyr, "(c) ", theManager);
	GetTextInfo(setupInfo->File, "FILE", theManager);

	return 0;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadTwinVQDLL();
}

Void smooth::DetachDLL()
{
	FreeTwinVQDLL();
}

Bool BoCA::DecoderTwinVQ::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".vqf");
}

Error BoCA::DecoderTwinVQ::GetStreamInfo(const String &streamURI, Track &track)
{
	BFILE	*bfp = 0;

	if (String::IsUnicode(streamURI))
	{
		File(streamURI).Copy(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"));

		bfp = bopen(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"), (char *) "rb");
	}
	else
	{
		bfp = bopen(streamURI, (char *) "rb");
	}

	/* Get setup info via header manager.
	 */
	CChunkChunk	*twinChunk	  = TvqGetBsHeaderInfo(bfp);
	CHeaderManager	*theHeaderManager = CHeaderManager::Create(*twinChunk);

	headerInfo	 setupInfo;

	TvqGetStandardChunkInfo(theHeaderManager, &setupInfo);

	delete theHeaderManager;
	delete twinChunk;

	/* Read file information.
	 */
	INDEX	 index;

	ex_TvqInitialize(&setupInfo, &index, 0);

	Format	 format = track.GetFormat();

	format.bits		= 16;
	format.rate		= ex_TvqGetSamplingRate();
	format.channels		= ex_TvqGetNumChannels();

	track.SetFormat(format);

	track.fileSize		= File(streamURI).GetFileSize();
	track.length		= -1;
	track.approxLength	= track.fileSize / (ex_TvqGetBitRate() * 1000 / 8) * format.rate;

	Info	 info = track.GetInfo();

	info.artist		= setupInfo.Auth;
	info.title		= setupInfo.Name;
	info.comment		= setupInfo.Comt;

	track.SetInfo(info);

	ex_TvqTerminate(&index);

	bclose(bfp);

	if (String::IsUnicode(streamURI))
	{
		File(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in")).Delete();
	}

	return Success();
}

BoCA::DecoderTwinVQ::DecoderTwinVQ()
{
	bfp	  = NIL;

	frameSize = -1;

	memset(&setupInfo, 0, sizeof(setupInfo));
	memset(&index, 0, sizeof(index));
}

BoCA::DecoderTwinVQ::~DecoderTwinVQ()
{
}

Bool BoCA::DecoderTwinVQ::Activate()
{
	if (String::IsUnicode(track.fileName))
	{
		File(track.fileName).Copy(Utilities::GetNonUnicodeTempFileName(track.fileName).Append(".in"));

		bfp = bopen(Utilities::GetNonUnicodeTempFileName(track.fileName).Append(".in"), (char *) "rb");
	}
	else
	{
		bfp = bopen(track.fileName, (char *) "rb");
	}

	/* Get setup info via header manager.
	 */
	CChunkChunk	*twinChunk	  = TvqGetBsHeaderInfo(bfp);
	CHeaderManager	*theHeaderManager = CHeaderManager::Create( *twinChunk );

	TvqGetStandardChunkInfo(theHeaderManager, &setupInfo);

	delete theHeaderManager;
	delete twinChunk;

	/* Initialize TwinVQ reader.
	 */
	ex_TvqInitialize(&setupInfo, &index, 0);

	TvqInitBsReader(&setupInfo);

	frameSize = ex_TvqGetFrameSize();
	frame.Resize(frameSize * track.GetFormat().channels);

	/* Two frames are cancelled after decoding start.
	 */
	TvqReadBsFrame(&index, bfp);
	ex_TvqDecodeFrame(&index, frame);

	TvqReadBsFrame(&index, bfp);
	ex_TvqDecodeFrame(&index, frame);

	return True;
}

Bool BoCA::DecoderTwinVQ::Deactivate()
{
	ex_TvqTerminate(&index);

	bclose(bfp);

	if (String::IsUnicode(track.fileName))
	{
		File(Utilities::GetNonUnicodeTempFileName(track.fileName).Append(".in")).Delete();
	}

	return True;
}

Int BoCA::DecoderTwinVQ::ReadData(Buffer<UnsignedByte> &data)
{
	if (TvqReadBsFrame(&index, bfp) == 0) return -1;

	ex_TvqDecodeFrame(&index, frame);

	data.Resize(frameSize * track.GetFormat().channels * 2);

	for (Int ch = 0; ch < track.GetFormat().channels; ch++)
	{
		for (Int i = 0; i < frameSize; i++)
		{
			register float sample = Math::Min(Math::Max((frame + ch * frameSize)[i], -32700.), 32700.);

			((short *) (UnsignedByte *) data)[i * track.GetFormat().channels + ch] = (short) (sample + (sample >= 0. ? 0.5 : -0.5));
		}
	}

	inBytes = ftell(bfp->fp);

	return data.Size();
}
