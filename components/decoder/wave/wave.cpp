 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifdef __WIN32__
#	include <windows.h>
#	include <mmreg.h>
#else
#	define WAVE_FORMAT_PCM	      0x0001
#	define WAVE_FORMAT_IEEE_FLOAT 0x0003
#	define WAVE_FORMAT_EXTENSIBLE 0xFFFE
#endif

#include "wave.h"

using namespace smooth::IO;

const String &BoCA::DecoderWave::GetComponentSpecs()
{
	static String	 componentSpecs = "				\
									\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
	  <component>							\
	    <name>Windows Wave File Input</name>			\
	    <version>1.0</version>					\
	    <id>wave-dec</id>						\
	    <type>decoder</type>					\
	    <format>							\
	      <name>Microsoft Wave Files</name>				\
	      <lossless>true</lossless>					\
	      <extension>wav</extension>				\
	      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>	\
	      <tag id=\"cart-tag\" mode=\"other\">RIFF Cart Tag</tag>	\
	      <tag id=\"id3v2-tag\" mode=\"other\">ID3v2</tag>		\
	    </format>							\
	  </component>							\
									\
	";

	return componentSpecs;
}

Bool BoCA::DecoderWave::CanOpenStream(const String &streamURI)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if (in.InputString(4) != "RIFF") return False;

	in.RelSeek(4);

	if (in.InputString(4) != "WAVE") return False;

	String		 chunk;

	while (chunk != "fmt ")
	{
		if (in.GetPos() >= in.Size()) break;

		/* Read next chunk.
		 */
		chunk = in.InputString(4);

		UnsignedInt32	 cSize = in.InputNumber(4);

		if (chunk == "fmt ")
		{
			Int	 waveFormat = in.InputNumber(2);

			if (waveFormat == WAVE_FORMAT_PCM	 ||
			    waveFormat == WAVE_FORMAT_IEEE_FLOAT ||
			    waveFormat == WAVE_FORMAT_EXTENSIBLE) return True;

			/* Skip rest of chunk.
			 */
			in.RelSeek(cSize - 2 + cSize % 2);
		}
		else
		{
			/* Skip chunk.
			 */
			in.RelSeek(cSize + cSize % 2);
		}
	}

	return False;
}

Error BoCA::DecoderWave::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	track.fileSize = in.Size();

	/* Read RIFF chunk.
	 */
	if (in.InputString(4) != "RIFF") { errorState = True; errorString = "Unknown file type"; }

	UnsignedInt32	 rSize = in.InputNumber(4);

	if (in.InputString(4) != "WAVE") { errorState = True; errorString = "Unknown file type"; }

	String		 chunk;

	while (!errorState)
	{
		if (in.GetPos() >= in.Size()) break;

		/* Read next chunk.
		 */
		chunk = in.InputString(4);

		UnsignedInt64	 cSize = in.InputNumber(4);

		if (chunk == "fmt ")
		{
			Int	 waveFormat = in.InputNumber(2);

			if (waveFormat != WAVE_FORMAT_PCM	 &&
			    waveFormat != WAVE_FORMAT_IEEE_FLOAT &&
			    waveFormat != WAVE_FORMAT_EXTENSIBLE) { errorState = True; errorString = "Unsupported audio format"; }

			Format	 format = track.GetFormat();

			format.channels	= (unsigned short) in.InputNumber(2);
			format.rate	= (unsigned long) in.InputNumber(4);

			in.RelSeek(6);

			format.fp	= (waveFormat == WAVE_FORMAT_IEEE_FLOAT);
			format.bits	= (unsigned short) in.InputNumber(2);
			format.order	= (waveFormat == WAVE_FORMAT_IEEE_FLOAT && format.bits == 64) ? BYTE_NATIVE : BYTE_INTEL;

			if (format.bits == 8) format.sign = False;

			track.SetFormat(format);

			/* Skip rest of chunk.
			 */
			in.RelSeek(cSize - 16 + cSize % 2);
		}
		else if (chunk == "data")
		{
			Format	 format = track.GetFormat();

			if (rSize == 0xFFFFFFFF || rSize == 0 ||
			    cSize == 0xFFFFFFFF || cSize == 0) cSize = in.Size() - in.GetPos();

			track.length	= cSize / format.channels / (format.bits / 8);
			format.bits	= Math::Min(32, format.bits);

			track.SetFormat(format);

			/* Skip rest of chunk.
			 */
			in.RelSeek(cSize + cSize % 2);
		}
		else if (chunk == "id3 ")
		{
			Buffer<UnsignedByte>	 buffer(cSize);

			in.InputData(buffer, cSize);

			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				tagger->SetConfiguration(GetConfiguration());
				tagger->ParseBuffer(buffer, track);

				boca.DeleteComponent(tagger);
			}

			/* Skip rest of chunk.
			 */
			in.RelSeek(cSize % 2);
		}
		else
		{
			/* Skip chunk.
			 */
			in.RelSeek(cSize + cSize % 2);
		}
	}

	if (!errorState)
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*cartTagger = (AS::TaggerComponent *) boca.CreateComponentByID("cart-tag");

		if (cartTagger != NIL)
		{
			cartTagger->SetConfiguration(GetConfiguration());
			cartTagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(cartTagger);
		}

		AS::TaggerComponent	*riffTagger = (AS::TaggerComponent *) boca.CreateComponentByID("riff-tag");

		if (riffTagger != NIL)
		{
			riffTagger->SetConfiguration(GetConfiguration());
			riffTagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(riffTagger);
		}
	}

	if (errorState)	return Error();
	else		return Success();
}

BoCA::DecoderWave::DecoderWave()
{
	floatFormat	= False;
	floatFormatBits	= 32;

	dataOffset	= 0;
}

BoCA::DecoderWave::~DecoderWave()
{
}

Bool BoCA::DecoderWave::Activate()
{
	InStream	 in(STREAM_DRIVER, driver);

	in.Seek(12);

	String		 chunk;

	do
	{
		/* Read next chunk.
		 */
		chunk = in.InputString(4);

		UnsignedInt32	 cSize = in.InputNumber(4);

		if (chunk == "fmt ")
		{
			Int	 waveFormat = in.InputNumber(2);

			if (waveFormat == WAVE_FORMAT_IEEE_FLOAT) floatFormat = True;

			in.RelSeek(12);

			floatFormatBits	= (unsigned short) in.InputNumber(2);

			/* Skip rest of chunk.
			 */
			in.RelSeek(cSize - 16 + cSize % 2);
		}
		else if (chunk != "data")
		{
			in.RelSeek(cSize + cSize % 2);
		}
	}
	while (chunk != "data");

	dataOffset = in.GetPos();

	driver->Seek(dataOffset);

	return True;
}

Bool BoCA::DecoderWave::Seek(Int64 samplePosition)
{
	const Format	&format = track.GetFormat();

	driver->Seek(dataOffset + samplePosition * format.channels * (format.bits / 8) * (floatFormatBits / 32));

	return True;
}

Int BoCA::DecoderWave::ReadData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	if (driver->GetPos() == driver->GetSize()) return -1;

	/* Read data.
	 */
	if (floatFormat && floatFormatBits == 64) data.Resize(data.Size() * 2);

	Int	 size = driver->ReadData(data, data.Size());

	/* Convert 64 bit float to 32 bit.
	 */
	if (floatFormat && floatFormatBits == 64)
	{
		if (endianness != EndianLittle) BoCA::Utilities::SwitchBufferByteOrder(data, 8);

		for (Int i = 0; i < size / 8; i++) ((Float32 *) (unsigned char *) data)[i] = ((Float64 *) (unsigned char *) data)[i];

		size /= 2;

		data.Resize(size);
	}

	return size;
}
