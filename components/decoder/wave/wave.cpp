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

		Int	 cSize = in.InputNumber(4);

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
	InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READ);

	track.fileSize	= f_in->Size();

	/* Read RIFF chunk.
	 */
	if (f_in->InputString(4) != "RIFF") { errorState = True; errorString = "Unknown file type"; }

	f_in->RelSeek(4);

	if (f_in->InputString(4) != "WAVE") { errorState = True; errorString = "Unknown file type"; }

	String		 chunk;

	while (!errorState && chunk != "data")
	{
		if (f_in->GetPos() >= f_in->Size()) break;

		/* Read next chunk.
		 */
		chunk = f_in->InputString(4);

		Int	 cSize = f_in->InputNumber(4);

		if (chunk == "fmt ")
		{
			Int	 waveFormat = f_in->InputNumber(2);

			if (waveFormat != WAVE_FORMAT_PCM	 &&
			    waveFormat != WAVE_FORMAT_IEEE_FLOAT &&
			    waveFormat != WAVE_FORMAT_EXTENSIBLE) { errorState = True; errorString = "Unsupported audio format"; }

			Format	 format = track.GetFormat();

			format.channels	= (unsigned short) f_in->InputNumber(2);
			format.rate	= (unsigned long) f_in->InputNumber(4);

			f_in->RelSeek(6);

			format.order	= (waveFormat == WAVE_FORMAT_IEEE_FLOAT) ? BYTE_NATIVE : BYTE_INTEL;
			format.bits	= (unsigned short) f_in->InputNumber(2);

			track.SetFormat(format);

			/* Skip rest of chunk.
			 */
			f_in->RelSeek(cSize - 16 + cSize % 2);
		}
		else if (chunk == "data")
		{
			Format	 format = track.GetFormat();

			track.length	= (unsigned long) cSize / format.channels / (format.bits / 8);
			format.bits	= Math::Min(32, format.bits);

			track.SetFormat(format);
		}
		else
		{
			/* Skip chunk.
			 */
			f_in->RelSeek(cSize + cSize % 2);
		}
	}

	delete f_in;

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
	packageSize	= 0;

	floatFormat	= False;
	floatFormatBits	= 32;

	dataOffset	= 0;
}

BoCA::DecoderWave::~DecoderWave()
{
}

Bool BoCA::DecoderWave::Activate()
{
	InStream	*in = new InStream(STREAM_DRIVER, driver);

	in->Seek(12);

	String		 chunk;

	do
	{
		/* Read next chunk.
		 */
		chunk = in->InputString(4);

		Int	 cSize = in->InputNumber(4);

		if (chunk == "fmt ")
		{
			Int	 waveFormat = in->InputNumber(2);

			if (waveFormat == WAVE_FORMAT_IEEE_FLOAT) floatFormat = True;

			in->RelSeek(12);

			floatFormatBits	= (unsigned short) in->InputNumber(2);

			/* Skip rest of chunk.
			 */
			in->RelSeek(cSize - 16 + cSize % 2);
		}
		else if (chunk != "data")
		{
			in->RelSeek(cSize + cSize % 2);
		}
	}
	while (chunk != "data");

	dataOffset = in->GetPos();

	delete in;

	driver->Seek(dataOffset);

	return True;
}

Bool BoCA::DecoderWave::Deactivate()
{
	return True;
}

Bool BoCA::DecoderWave::Seek(Int64 samplePosition)
{
	driver->Seek(dataOffset + samplePosition * track.GetFormat().channels * (track.GetFormat().bits / 8));

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

	/* Convert float to integer.
	 */
	if (floatFormat && floatFormatBits == 32)
	{
		if (endianness != EndianLittle) BoCA::Utilities::SwitchBufferByteOrder(data, 4);

		for (Int i = 0; i < size / 4; i++) ((Int32 *) (unsigned char *) data)[i] = Math::Min(Int64( 0x7FFFFFFF),
											   Math::Max(Int64(~0x7FFFFFFF), Int64(((ShortFloat *) (unsigned char *) data)[i] * 0x80000000)));
	}
	else if (floatFormat && floatFormatBits == 64)
	{
		if (endianness != EndianLittle) BoCA::Utilities::SwitchBufferByteOrder(data, 8);

		for (Int i = 0; i < size / 8; i++) ((Int32 *) (unsigned char *) data)[i] = Math::Min(Int64( 0x7FFFFFFF),
											   Math::Max(Int64(~0x7FFFFFFF), Int64(((Float *)      (unsigned char *) data)[i] * 0x80000000)));

		size /= 2;

		data.Resize(size);
	}

	return size;
}
