 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2023 Robert Kausch <robert.kausch@freac.org>
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

#include "mac.h"

using namespace smooth::IO;

const String &BoCA::DecoderMAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (macdll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>Monkey's Audio Decoder %VERSION%</name>		\
		    <version>1.0</version>					\
		    <id>mac-dec</id>						\
		    <type>decoder</type>					\
		    <replace>ffmpeg-mac-dec</replace>				\
		    <format>							\
		      <name>Monkey's Audio</name>				\
		      <lossless>true</lossless>					\
		      <extension>ape</extension>				\
		      <extension>mac</extension>				\
		      <tag id=\"apev2-tag\" mode=\"append\">APEv2</tag>		\
		    </format>							\
		  </component>							\
										\
		";

		componentSpecs.Replace("%VERSION%", String("v").Append(ex_GetLibraryVersionString()));
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadMACDLL();
}

Void smooth::DetachDLL()
{
	FreeMACDLL();
}

Bool BoCA::DecoderMAC::CanOpenStream(const String &streamURI)
{
	String	 lcURI = streamURI.ToLower();

	return lcURI.EndsWith(".ape") ||
	       lcURI.EndsWith(".mac");
}

Error BoCA::DecoderMAC::GetStreamInfo(const String &streamURI, Track &track)
{
	APE_DECOMPRESS_HANDLE	 hAPEDecompress = ex_APEDecompress_CreateW(streamURI, NIL);

	if (hAPEDecompress == NIL) return Error();

	Format	 format = track.GetFormat();

	format.fp	= ex_APEDecompress_GetInfo(hAPEDecompress, IAPEDecompress::APE_INFO_FORMAT_FLAGS, 0, 0) & MAC_FORMAT_FLAG_FLOATING_POINT;
	format.bits	= ex_APEDecompress_GetInfo(hAPEDecompress, IAPEDecompress::APE_INFO_BITS_PER_SAMPLE, 0, 0);
	format.channels	= ex_APEDecompress_GetInfo(hAPEDecompress, IAPEDecompress::APE_INFO_CHANNELS, 0, 0);
	format.rate	= ex_APEDecompress_GetInfo(hAPEDecompress, IAPEDecompress::APE_INFO_SAMPLE_RATE, 0, 0);

	format.sign	= format.bits != 8;

	track.length	= ex_APEDecompress_GetInfo(hAPEDecompress, IAPEDecompress::APE_DECOMPRESS_TOTAL_BLOCKS, 0, 0);

	track.SetFormat(format);

	ex_APEDecompress_Destroy(hAPEDecompress);

	track.fileSize	= File(streamURI).GetFileSize();

	/* Parse APE tag if present.
	 */
	AS::Registry		&boca = AS::Registry::Get();
	AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("apev2-tag");

	if (tagger != NIL)
	{
		tagger->SetConfiguration(GetConfiguration());
		tagger->ParseStreamInfo(streamURI, track);

		boca.DeleteComponent(tagger);
	}

	return Success();
}

BoCA::DecoderMAC::DecoderMAC()
{
	hAPEDecompress = NIL;

	blockId	       = 0;
}

BoCA::DecoderMAC::~DecoderMAC()
{
}

Bool BoCA::DecoderMAC::Activate()
{
	hAPEDecompress = ex_APEDecompress_CreateW(track.fileName, NIL);
	blockId	       = 0;

	return True;
}

Bool BoCA::DecoderMAC::Deactivate()
{
	ex_APEDecompress_Destroy(hAPEDecompress);

	return True;
}

Bool BoCA::DecoderMAC::Seek(Int64 samplePosition)
{
	if (ex_APEDecompress_Seek(hAPEDecompress, samplePosition) == ERROR_SUCCESS) return True;
	else									    return False;
}

Int BoCA::DecoderMAC::ReadData(Buffer<UnsignedByte> &data)
{
	inBytes += data.Size();

	int64	 nBlockAlign	       = ex_APEDecompress_GetInfo(hAPEDecompress, IAPEDecompress::APE_INFO_BLOCK_ALIGN, 0, 0);
	int64	 nBlocksRetrieved      = 0;
	int64	 nTotalBlocksRetrieved = 0;

	do
	{
		data.Resize((nTotalBlocksRetrieved + 1024) * nBlockAlign);

		/* Try to decompress 1024 blocks
		 */
		if (ex_APEDecompress_GetData(hAPEDecompress, (char *) (unsigned char *) data + nTotalBlocksRetrieved * nBlockAlign, 1024, &nBlocksRetrieved) != ERROR_SUCCESS) return -1;

		nTotalBlocksRetrieved += nBlocksRetrieved;
		blockId += nBlocksRetrieved;
	}
	while (nBlocksRetrieved > 0 && blockId < (track.length * (double(inBytes) / track.fileSize)));

	return nTotalBlocksRetrieved * nBlockAlign;
}
