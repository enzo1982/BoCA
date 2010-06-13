 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "mac_in.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::MACIn::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (macdll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>Monkey's Audio Decoder</name>		\
		    <version>1.0</version>			\
		    <id>mac-in</id>				\
		    <type>decoder</type>			\
		    <format>					\
		      <name>Monkey's Audio</name>		\
		      <extension>ape</extension>		\
		      <extension>mac</extension>		\
		      <tag mode=\"append\">APEv2</tag>		\
		    </format>					\
		  </component>					\
								\
		";
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

Bool BoCA::MACIn::CanOpenStream(const String &streamURI)
{
	String	 lcURI = streamURI.ToLower();

	return lcURI.EndsWith(".ape") ||
	       lcURI.EndsWith(".mac");
}

Error BoCA::MACIn::GetStreamInfo(const String &streamURI, Track &track)
{
	int			 nRetVal = 0;
	APE_DECOMPRESS_HANDLE	 hAPEDecompress = NIL;

	if (String::IsUnicode(streamURI))
	{
		File(streamURI).Copy(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in.ape"));

		hAPEDecompress = ex_APEDecompress_Create(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in.ape"), &nRetVal);
	}
	else
	{
		hAPEDecompress = ex_APEDecompress_Create(streamURI, &nRetVal);
	}

	if (hAPEDecompress == NIL) return Error();

	Format	&format = track.GetFormat();

	format.order	= BYTE_INTEL;
	format.bits	= ex_APEDecompress_GetInfo(hAPEDecompress, APE_INFO_BITS_PER_SAMPLE, 0, 0);

	format.channels	= ex_APEDecompress_GetInfo(hAPEDecompress, APE_INFO_CHANNELS, 0, 0);
	format.rate	= ex_APEDecompress_GetInfo(hAPEDecompress, APE_INFO_SAMPLE_RATE, 0, 0);

	track.length	= ex_APEDecompress_GetInfo(hAPEDecompress, APE_DECOMPRESS_TOTAL_BLOCKS, 0, 0) * format.channels;

	ex_APEDecompress_Destroy(hAPEDecompress);

	track.fileSize	= File(streamURI).GetFileSize();

	/* Parse APE tag if present.
	 */
	AS::Registry		&boca = AS::Registry::Get();
	AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("apev2-tag");

	if (tagger != NIL)
	{
		tagger->ParseStreamInfo(streamURI, track);

		boca.DeleteComponent(tagger);
	}

	if (String::IsUnicode(streamURI))
	{
		File(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in.ape")).Delete();
	}

	return Success();
}

BoCA::MACIn::MACIn()
{
	hAPEDecompress = NIL;

	packageSize = 0;
}

BoCA::MACIn::~MACIn()
{
}

Bool BoCA::MACIn::Activate()
{
	int	 nRetVal = 0;

	if (String::IsUnicode(track.origFilename))
	{
		File(track.origFilename).Copy(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in.ape"));

		hAPEDecompress = ex_APEDecompress_Create(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in.ape"), &nRetVal);
	}
	else
	{
		hAPEDecompress = ex_APEDecompress_Create(track.origFilename, &nRetVal);
	}

	blockId = 0;

	return True;
}

Bool BoCA::MACIn::Deactivate()
{
	ex_APEDecompress_Destroy(hAPEDecompress);

	if (String::IsUnicode(track.origFilename))
	{
		File(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in.ape")).Delete();
	}

	return True;
}

Int BoCA::MACIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (size <= 0) return -1;

	inBytes += size;

	int	 nBlockAlign = ex_APEDecompress_GetInfo(hAPEDecompress, APE_INFO_BLOCK_ALIGN, 0, 0);
	int	 nTotalBlocks = ex_APEDecompress_GetInfo(hAPEDecompress, APE_DECOMPRESS_TOTAL_BLOCKS, 0, 0);
	int	 nBlocksRetrieved = 0;
	int	 nTotalBlocksRetrieved = 0;

	do
	{
		data.Resize((nTotalBlocksRetrieved + 1024) * nBlockAlign);

		/* Try to decompress 1024 blocks
		 */
		ex_APEDecompress_GetData(hAPEDecompress, (char *) (unsigned char *) data + nTotalBlocksRetrieved * nBlockAlign, 1024, &nBlocksRetrieved);

		nTotalBlocksRetrieved += nBlocksRetrieved;
		blockId += nBlocksRetrieved;
	}
	while (nBlocksRetrieved > 0 && blockId < (nTotalBlocks * (double(inBytes) / track.fileSize)));

	return nTotalBlocksRetrieved * nBlockAlign;
}
