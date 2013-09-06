 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifdef __WIN32__
#	include <windows.h>
#	include <mmreg.h>
#else
#	define WAVE_FORMAT_PCM	      0x0001
#	define WAVE_FORMAT_EXTENSIBLE 0xFFFE
#endif

#include "wave_in.h"

using namespace smooth::IO;

const String &BoCA::WaveIn::GetComponentSpecs()
{
	static String	 componentSpecs = "				\
									\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
	  <component>							\
	    <name>Windows Wave File Input</name>			\
	    <version>1.0</version>					\
	    <id>wave-in</id>						\
	    <type>decoder</type>					\
	    <format>							\
	      <name>Windows Wave Files</name>				\
	      <extension>wav</extension>				\
	      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>	\
	      <tag id=\"cart-tag\" mode=\"other\">RIFF Cart Tag</tag>	\
	    </format>							\
	  </component>							\
									\
	";

	return componentSpecs;
}

Bool BoCA::WaveIn::CanOpenStream(const String &streamURI)
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

			if (waveFormat == WAVE_FORMAT_PCM ||
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

Error BoCA::WaveIn::GetStreamInfo(const String &streamURI, Track &track)
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

			if (waveFormat != WAVE_FORMAT_PCM &&
			    waveFormat != WAVE_FORMAT_EXTENSIBLE) { errorState = True; errorString = "Unsupported audio format"; }

			Format	 format = track.GetFormat();

			format.channels	= (unsigned short) f_in->InputNumber(2);
			format.rate	= (unsigned long) f_in->InputNumber(4);

			f_in->RelSeek(6);

			format.order	= BYTE_INTEL;
			format.bits	= (unsigned short) f_in->InputNumber(2);

			track.SetFormat(format);

			/* Skip rest of chunk.
			 */
			f_in->RelSeek(cSize - 16 + cSize % 2);
		}
		else if (chunk == "data")
		{
			track.length	= (unsigned long) cSize / track.GetFormat().channels / (track.GetFormat().bits / 8);
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
			cartTagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(cartTagger);
		}

		AS::TaggerComponent	*riffTagger = (AS::TaggerComponent *) boca.CreateComponentByID("riff-tag");

		if (riffTagger != NIL)
		{
			riffTagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(riffTagger);
		}
	}

	if (errorState)	return Error();
	else		return Success();
}

BoCA::WaveIn::WaveIn()
{
	packageSize = 0;

	dataOffset  = 0;
}

BoCA::WaveIn::~WaveIn()
{
}

Bool BoCA::WaveIn::Activate()
{
	InStream	*in = new InStream(STREAM_DRIVER, driver);
    
	in->Seek(12);

	String		 chunk;

	do
	{
		/* Read next chunk
		 */
		chunk = in->InputString(4);

		Int	 cSize = in->InputNumber(4);

		if (chunk != "data") in->RelSeek(cSize + cSize % 2);
	}
	while (chunk != "data");

	dataOffset = in->GetPos();

	delete in;

	driver->Seek(dataOffset);

	return True;
}

Bool BoCA::WaveIn::Deactivate()
{
	return True;
}

Bool BoCA::WaveIn::Seek(Int64 samplePosition)
{
	driver->Seek(dataOffset + samplePosition * track.GetFormat().channels * (track.GetFormat().bits / 8));

	return True;
}

Int BoCA::WaveIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (driver->GetPos() == driver->GetSize()) return -1;

	data.Resize(size);

	size = driver->ReadData(data, size);

	return size;
}
