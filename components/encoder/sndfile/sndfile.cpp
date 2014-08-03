 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "sndfile.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderSndFile::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (sndfiledll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>SndFile Output Component</name>			\
		    <version>1.0</version>					\
		    <id>sndfile-enc</id>					\
		    <type>encoder</type>					\
		    <replace>wave-enc</replace>					\
		    <format>							\
		      <name>Windows Wave Files</name>				\
		      <extension>wav</extension>				\
		      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>	\
		      <tag id=\"cart-tag\" mode=\"other\">RIFF Cart Tag</tag>	\
		    </format>							\
		    <format>							\
		      <name>Apple Audio Files</name>				\
		      <extension>aif</extension>				\
		      <extension>aiff</extension>				\
		      <extension>aifc</extension>				\
		      <tag id=\"id3v2-tag\" mode=\"other\">ID3v2</tag>		\
		    </format>							\
		    <format>							\
		      <name>Sun Audio Files</name>				\
		      <extension>au</extension>					\
		      <extension>snd</extension>				\
		    </format>							\
		    <format>							\
		      <name>Paris Audio Files</name>				\
		      <extension>paf</extension>				\
		    </format>							\
		    <format>							\
		      <name>Amiga Audio Files</name>				\
		      <extension>iff</extension>				\
		      <extension>svx</extension>				\
		    </format>							\
		    <format>							\
		      <name>IRCAM Sound Files</name>				\
		      <extension>sf</extension>					\
		    </format>							\
		    <format>							\
		      <name>Creative Voice Files</name>				\
		      <extension>voc</extension>				\
		    </format>							\
		    <format>							\
		      <name>Sound Forge Wave64</name>				\
		      <extension>w64</extension>				\
		      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>	\
		    </format>							\
		    <format>							\
		      <name>RIFF 64 Audio Files</name>				\
		      <extension>rf64</extension>				\
		      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>	\
		      <tag id=\"cart-tag\" mode=\"other\">RIFF Cart Tag</tag>	\
		    </format>							\
		    <format>							\
		      <name>Portable Voice Format</name>			\
		      <extension>pvf</extension>				\
		    </format>							\
		    <format>							\
		      <name>HMM Toolkit Format</name>				\
		      <extension>htk</extension>				\
		    </format>							\
		    <format>							\
		      <name>Apple Core Audio</name>				\
		      <extension>caf</extension>				\
		    </format>							\
		    <format>							\
		      <name>Psion WVE Format</name>				\
		      <extension>wve</extension>				\
		    </format>							\
		    <format>							\
		      <name>Audio Visual Research Format</name>			\
		      <extension>avr</extension>				\
		    </format>							\
		  </component>							\
										\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadSndFileDLL();
}

Void smooth::DetachDLL()
{
	FreeSndFileDLL();
}

BoCA::EncoderSndFile::EncoderSndFile()
{
	configLayer = NIL;

	file	    = 0;
	sndf	    = NIL;
}

BoCA::EncoderSndFile::~EncoderSndFile()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderSndFile::Activate()
{
	Config		*config = Config::Get();
	const Format	&format = track.GetFormat();

	/* Open output file.
	 */
	String	 fileName = Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"); 

#ifdef __WIN32__
	if (Setup::enableUnicode) file = _wfopen(fileName, L"wb");
	else			  file = fopen(fileName, "wb");
#else
	file = fopen(fileName.ConvertTo("UTF-8"), "wb");
#endif

	if (file == NIL) return False;

	/* Get selected file format.
	 */
#ifdef __APPLE__
	Int	 fileFormat = config->GetIntValue("SndFile", "Format", SF_FORMAT_AIFF);
#else
	Int	 fileFormat = config->GetIntValue("SndFile", "Format", SF_FORMAT_WAV);
#endif

	/* Fill format info structure.
	 */
	SF_INFO	 sinfo;

	memset(&sinfo, 0, sizeof(SF_INFO));

	sinfo.format	 = fileFormat | config->GetIntValue("SndFile", "SubFormat", 0);
	sinfo.samplerate = format.rate;
	sinfo.channels	 = format.channels;

	if (config->GetIntValue("SndFile", "SubFormat", 0) == 0) sinfo.format |= SelectBestSubFormat(format, fileFormat);

	/* Check if selected format is supported.
	 */
	if (!ex_sf_format_check(&sinfo))
	{
		I18n	*i18n = I18n::Get();

		i18n->SetContext("Encoders::SndFile::Errors");

		if (config->GetIntValue("SndFile", "SubFormat", 0) == 0) errorString = i18n->TranslateString("Could not find suitable audio format.");
		else							 errorString = i18n->TranslateString("Unable to use selected audio format.");

		errorState = True;

		fclose(file);

		return False;
	}

	/* Open sndfile handle.
	 */
	sndf = ex_sf_open_fd(fileno(file), SFM_WRITE, &sinfo, False);

	if (sndf == NIL) { fclose(file); return False; }

	ex_sf_command(sndf, SFC_SET_SCALE_INT_FLOAT_WRITE, NIL, SF_TRUE);

	/* Set metadata.
	 */
	if (fileFormat != SF_FORMAT_WAV && fileFormat != SF_FORMAT_RF64 && fileFormat != SF_FORMAT_W64)
	{
		Info	 info = track.GetInfo();

		if (info.artist != NIL || info.title != NIL)
		{
			if (info.artist	!= NIL) ex_sf_set_string(sndf, SF_STR_ARTIST, info.artist);
			if (info.title	!= NIL) ex_sf_set_string(sndf, SF_STR_TITLE, info.title);
			if (info.album	!= NIL) ex_sf_set_string(sndf, SF_STR_ALBUM, info.album);
			if (info.track	 >   0) ex_sf_set_string(sndf, SF_STR_TRACKNUMBER, String::FromInt(info.track));
			if (info.year	 >   0) ex_sf_set_string(sndf, SF_STR_DATE, String::FromInt(info.year));
			if (info.genre	!= NIL) ex_sf_set_string(sndf, SF_STR_GENRE, info.genre);

			if	(info.comment != NIL && !config->GetIntValue("Tags", "ReplaceExistingComments", False))	ex_sf_set_string(sndf, SF_STR_COMMENT, info.comment);
			else if (config->GetStringValue("Tags", "DefaultComment", NIL) != NIL)				ex_sf_set_string(sndf, SF_STR_COMMENT, config->GetStringValue("Tags", "DefaultComment", NIL));
		}
	}

	return True;
}

Bool BoCA::EncoderSndFile::Deactivate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	/* Close sndfile handle.
	 */
	ex_sf_close(sndf);

	fclose(file);

	/* Stream contents of created file to output driver
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

	/* Write metadata.
	 */
	Config		*config = Config::Get();
	const Info	&info	= track.GetInfo();

#ifdef __APPLE__
	Int	 fileFormat = config->GetIntValue("SndFile", "Format", SF_FORMAT_AIFF);
#else
	Int	 fileFormat = config->GetIntValue("SndFile", "Format", SF_FORMAT_WAV);
#endif

	if (fileFormat == SF_FORMAT_WAV || fileFormat == SF_FORMAT_RF64)
	{
		/* Write RIFF tag if requested.
		 */
		if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableRIFFINFOTag", True))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("riff-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 tagBuffer;

				tagger->RenderBuffer(tagBuffer, track);

				driver->WriteData(tagBuffer, tagBuffer.Size());

				boca.DeleteComponent(tagger);
			}
		}

		/* Write CART tag if requested.
		 */
		if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableRIFFCartTag", True))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("cart-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 tagBuffer;

				tagger->RenderBuffer(tagBuffer, track);

				driver->WriteData(tagBuffer, tagBuffer.Size());

				boca.DeleteComponent(tagger);
			}
		}

		/* Write file size to header.
		 */
		Int64	 fileSize = driver->GetSize() - 8;

		if (fileFormat == SF_FORMAT_WAV)
		{
			if (fileSize >= (Int64(1) << 32)) fileSize = -1;

			driver->Seek(4);

			if (endianness == EndianLittle)	for (Int i = 0; i <= 3; i++) driver->WriteData(((unsigned char *) &fileSize) + i, 1);
			else				for (Int i = 7; i >= 4; i--) driver->WriteData(((unsigned char *) &fileSize) + i, 1);
		}
		else
		{
			driver->Seek(20);

			if (endianness == EndianLittle)	for (Int i = 0; i <= 7; i++) driver->WriteData(((unsigned char *) &fileSize) + i, 1);
			else				for (Int i = 7; i >= 0; i--) driver->WriteData(((unsigned char *) &fileSize) + i, 1);
		}

		driver->Seek(driver->GetSize());
	}
	else if (fileFormat == SF_FORMAT_W64)
	{
		/* Write RIFF tag if requested.
		 */
		if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableRIFFINFOTag", True))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("riff-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 tagBuffer;

				tagger->RenderBuffer(tagBuffer, track);

				unsigned char	 guid[16] = { 'l', 'i', 's', 't', 0x2F, 0x91, 0xCF, 0x11, 0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00 };
				Int64		 size	  = tagBuffer.Size() + 16;

				driver->WriteData(guid, 16);

				if (endianness == EndianLittle)	for (Int i = 0; i <= 7; i++) driver->WriteData(((unsigned char *) &size) + i, 1);
				else				for (Int i = 7; i >= 0; i--) driver->WriteData(((unsigned char *) &size) + i, 1);

				driver->WriteData(tagBuffer + 8, tagBuffer.Size() - 8);

				if (size % 8 > 0) for (Int i = 0; i < 8 - (size % 8); i++) driver->WriteData((unsigned char *) "", 1);

				boca.DeleteComponent(tagger);
			}
		}

		/* Write file size to header.
		 */
		Int64	 fileSize = driver->GetSize();

		driver->Seek(16);

		if (endianness == EndianLittle)	for (Int i = 0; i <= 7; i++) driver->WriteData(((unsigned char *) &fileSize) + i, 1);
		else				for (Int i = 7; i >= 0; i--) driver->WriteData(((unsigned char *) &fileSize) + i, 1);

		driver->Seek(driver->GetSize());
	}
	else if (fileFormat == SF_FORMAT_AIFF)
	{
		/* Write ID3v2 tag if requested.
		 */
		if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableID3v2", True))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->RenderBuffer(id3Buffer, track);

				driver->WriteData((unsigned char *) "ID3 ", 4);

				Int	 size = id3Buffer.Size();

				if (endianness == EndianLittle)	for (Int i = 3; i >= 0; i--) driver->WriteData(((unsigned char *) &size) + i, 1);
				else				for (Int i = 0; i <= 3; i++) driver->WriteData(((unsigned char *) &size) + i, 1);

				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		} 

		/* Write file size to header.
		 */
		Int64	 fileSize = driver->GetSize() - 8;

		if (fileSize >= (Int64(1) << 32)) fileSize = -1;

		driver->Seek(4);

		if (endianness == EndianLittle)	for (Int i = 3; i >= 0; i--) driver->WriteData(((unsigned char *) &fileSize) + i, 1);
		else				for (Int i = 4; i <= 7; i++) driver->WriteData(((unsigned char *) &fileSize) + i, 1);

		driver->Seek(driver->GetSize());
	}

	return True;
}

Int BoCA::EncoderSndFile::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format = track.GetFormat();
	int		 bytes	= 0;

	data.Resize(size);

	if (format.bits == 8)
	{
		Buffer<short>	 buffer(size);

		for (Int i = 0; i < size; i++) buffer[i] = (data[i] - 128) << 8;

		bytes = ex_sf_write_short(sndf, buffer, size);
	}
	else if	(format.bits == 16)
	{
		bytes = ex_sf_write_short(sndf, (short *) (UnsignedByte *) data, size / 2) * 2;
	}
	else if (format.bits == 24)
	{
		Buffer<int>	 buffer(size / 3);

		for (Int i = 0; i < size / 3; i++)
		{
			if (endianness == EndianLittle)	{ buffer[i] = (int) (data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8); }
			else				{ buffer[i] = (int) (data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8); }
		}

		bytes = ex_sf_write_int(sndf, buffer, size / 3) * 3;
	}
	else if (format.bits == 32)
	{
		bytes = ex_sf_write_int(sndf, (int *) (UnsignedByte *) data, size / 4) * 4;
	}

	return bytes;
}

Int BoCA::EncoderSndFile::SelectBestSubFormat(const Format &format, Int fileFormat)
{
	static Int	 formats8Bit[]	= { SF_FORMAT_PCM_S8, SF_FORMAT_PCM_U8, SF_FORMAT_PCM_16, SF_FORMAT_PCM_24, SF_FORMAT_PCM_32, SF_FORMAT_FLOAT, SF_FORMAT_DOUBLE, SF_FORMAT_ULAW, SF_FORMAT_ALAW, NIL };
	static Int	 formats16Bit[] = { SF_FORMAT_PCM_16, SF_FORMAT_PCM_24, SF_FORMAT_PCM_32, SF_FORMAT_FLOAT, SF_FORMAT_DOUBLE, SF_FORMAT_PCM_S8, SF_FORMAT_PCM_U8, SF_FORMAT_ULAW, SF_FORMAT_ALAW, NIL };
	static Int	 formats24Bit[] = { SF_FORMAT_PCM_24, SF_FORMAT_PCM_32, SF_FORMAT_FLOAT, SF_FORMAT_DOUBLE, SF_FORMAT_PCM_16, SF_FORMAT_PCM_S8, SF_FORMAT_PCM_U8, SF_FORMAT_ULAW, SF_FORMAT_ALAW, NIL };
	static Int	 formats32Bit[] = { SF_FORMAT_PCM_32, SF_FORMAT_FLOAT, SF_FORMAT_DOUBLE, SF_FORMAT_PCM_24, SF_FORMAT_PCM_16, SF_FORMAT_PCM_S8, SF_FORMAT_PCM_U8, SF_FORMAT_ULAW, SF_FORMAT_ALAW, NIL };

	Int	*formats = NIL;

	if	(format.bits ==  8) formats = formats8Bit;
	else if (format.bits == 16) formats = formats16Bit;
	else if (format.bits == 24) formats = formats24Bit;
	else if (format.bits == 32) formats = formats32Bit;

	for (Int i = 0; formats[i] != NIL; i++)
	{
		SF_INFO		 info;

		info.samplerate = format.rate;
		info.channels	= format.channels;
		info.format	= fileFormat | formats[i];

		if (ex_sf_format_check(&info)) return formats[i];
	}

	return 0;
}

String BoCA::EncoderSndFile::GetOutputFileExtension()
{
	Config	*config = Config::Get();

	SF_FORMAT_INFO	 format_info;

#ifdef __APPLE__
	format_info.format = config->GetIntValue("SndFile", "Format", SF_FORMAT_AIFF);
#else
	format_info.format = config->GetIntValue("SndFile", "Format", SF_FORMAT_WAV);
#endif

	ex_sf_command(NIL, SFC_GET_FORMAT_INFO, &format_info, sizeof(format_info));

	return format_info.extension;
}

ConfigLayer *BoCA::EncoderSndFile::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureSndFile();

	return configLayer;
}
