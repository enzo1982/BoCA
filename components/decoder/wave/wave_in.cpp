 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

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
	    </format>							\
	  </component>							\
									\
	";

	return componentSpecs;
}

Bool BoCA::WaveIn::CanOpenStream(const String &streamURI)
{
	InStream	*f_in	 = new InStream(STREAM_FILE, streamURI, IS_READ);
	Int		 magic = f_in->InputNumber(4);

	delete f_in;

	return (magic == 1179011410);
}

Error BoCA::WaveIn::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	*f_in	 = new InStream(STREAM_FILE, streamURI, IS_READ);

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
			if (f_in->InputNumber(2) != 1) { errorState = True; errorString = "Unsupported audio format"; }

			Format	 format = track.GetFormat();

			format.channels	= (unsigned short) f_in->InputNumber(2);
			format.rate	= (unsigned long) f_in->InputNumber(4);

			f_in->RelSeek(6);

			format.order	= BYTE_INTEL;
			format.bits	= (unsigned short) f_in->InputNumber(2);

			track.SetFormat(format);

			/* Skip rest of chunk.
			 */
			f_in->RelSeek(cSize - 16);
		}
		else if (chunk == "data")
		{
			track.length	= (unsigned long) cSize / (track.GetFormat().bits / 8);
		}
		else
		{
			/* Skip chunk.
			 */
			f_in->RelSeek(cSize);
		}
	}

	delete f_in;

	if (!errorState)
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("riff-tag");

		if (tagger != NIL)
		{
			tagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(tagger);
		}
	}

	if (errorState)	return Error();
	else		return Success();
}

BoCA::WaveIn::WaveIn()
{
	packageSize = 0;
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

		if (chunk != "data") in->RelSeek(cSize);
	}
	while (chunk != "data");

	driver->Seek(in->GetPos());

	delete in;

	return True;
}

Bool BoCA::WaveIn::Deactivate()
{
	return True;
}

Int BoCA::WaveIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (driver->GetPos() == driver->GetSize()) return -1;

	data.Resize(size);

	size = driver->ReadData(data, size);

	return size;
}
