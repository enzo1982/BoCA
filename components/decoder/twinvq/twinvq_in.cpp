 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

using namespace smooth::IO;

#include "twinvq_in.h"
#include "dllinterface.h"

#include <iostream>

#include "twinvq/bfile.h"
#include "twinvq/bstream.h"
#include "twinvq/HeaderManager.h"

const String &BoCA::TwinVQIn::GetComponentSpecs()
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
		    <id>twinvq-in</id>				\
		    <type>decoder</type>			\
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
		sprintf(data, "");

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

Bool BoCA::TwinVQIn::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".vqf");
}

Error BoCA::TwinVQIn::GetStreamInfo(const String &streamURI, Track &track)
{
	if (String::IsUnicode(streamURI))
	{
		File(streamURI).Copy(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"));

		bfp = bopen(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"), (char *) "rb");
	}
	else
	{
		bfp = bopen(streamURI, (char *) "rb");
	}

	CChunkChunk	*twinChunk = TvqGetBsHeaderInfo(bfp);
	CHeaderManager	*theHeaderManager = CHeaderManager::Create(*twinChunk);

	TvqGetStandardChunkInfo(theHeaderManager, &setupInfo);

	ex_TvqInitialize(&setupInfo, &index, 0);

	Format	 format = track.GetFormat();

	format.order		= BYTE_INTEL;
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

BoCA::TwinVQIn::TwinVQIn()
{
	packageSize = 0;
}

BoCA::TwinVQIn::~TwinVQIn()
{
}

Bool BoCA::TwinVQIn::Activate()
{
	if (String::IsUnicode(track.origFilename))
	{
		File(track.origFilename).Copy(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in"));

		bfp = bopen(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in"), (char *) "rb");
	}
	else
	{
		bfp = bopen(track.origFilename, (char *) "rb");
	}

	CChunkChunk	*twinChunk = TvqGetBsHeaderInfo(bfp);
	CHeaderManager	*theHeaderManager = CHeaderManager::Create( *twinChunk );

	TvqGetStandardChunkInfo(theHeaderManager, &setupInfo);

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

Bool BoCA::TwinVQIn::Deactivate()
{
	ex_TvqTerminate(&index);

	bclose(bfp);

	if (String::IsUnicode(track.origFilename))
	{
		File(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in")).Delete();
	}

	return True;
}

Int BoCA::TwinVQIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (TvqReadBsFrame(&index, bfp) != 0)
	{
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
	}
	else
	{
		return -1;
	}

	inBytes = ftell(bfp->fp);

	return data.Size();
}
