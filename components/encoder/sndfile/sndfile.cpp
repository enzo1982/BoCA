 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
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

#include "sndfile.h"
#include "config.h"

const String &BoCA::EncoderSndFile::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (sndfiledll != NIL)
	{
		componentSpecs = "								\
												\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>					\
		  <component>									\
		    <name>SndFile Output Component %VERSION%</name>				\
		    <version>1.0</version>							\
		    <id>sndfile-enc</id>							\
		    <type>encoder</type>							\
		    <replace>wave-enc</replace>							\
		    <format>									\
		      <name>Microsoft Wave Files</name>						\
		      <extension>wav</extension>						\
		      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>			\
		      <tag id=\"cart-tag\" mode=\"other\">RIFF Cart Tag</tag>			\
		      <tag id=\"id3v2-tag\" mode=\"other\">ID3v2</tag>				\
		    </format>									\
		    <format>									\
		      <name>Apple Audio Files</name>						\
		      <extension>aif</extension>						\
		      <extension>aiff</extension>						\
		      <extension>aifc</extension>						\
		      <tag id=\"id3v2-tag\" mode=\"other\">ID3v2</tag>				\
		    </format>									\
		    <format>									\
		      <name>Apple Core Audio Files</name>					\
		      <extension>caf</extension>						\
		    </format>									\
		    <format>									\
		      <name>Sony Media Wave64 Files</name>					\
		      <extension>w64</extension>						\
		      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>			\
		    </format>									\
		    <format>									\
		      <name>RIFF 64 Audio Files</name>						\
		      <extension>rf64</extension>						\
		      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>			\
		      <tag id=\"cart-tag\" mode=\"other\">RIFF Cart Tag</tag>			\
		    </format>									\
		    <format>									\
		      <name>Sun Audio Files</name>						\
		      <extension>au</extension>							\
		      <extension>snd</extension>						\
		    </format>									\
		    <format>									\
		      <name>Creative Voice Files</name>						\
		      <extension>voc</extension>						\
		    </format>									\
		    <format>									\
		      <name>Amiga Audio Files</name>						\
		      <extension>iff</extension>						\
		      <extension>svx</extension>						\
		    </format>									\
		    <format>									\
		      <name>IRCAM Sound Files</name>						\
		      <extension>sf</extension>							\
		    </format>									\
		    <format>									\
		      <name>Paris Audio Files</name>						\
		      <extension>paf</extension>						\
		    </format>									\
		    <format>									\
		      <name>Portable Voice Format</name>					\
		      <extension>pvf</extension>						\
		    </format>									\
		    <format>									\
		      <name>Psion WVE Files</name>						\
		      <extension>wve</extension>						\
		    </format>									\
		    <format>									\
		      <name>HMM Toolkit Format</name>						\
		      <extension>htk</extension>						\
		    </format>									\
		    <format>									\
		      <name>Audio Visual Research Format</name>					\
		      <extension>avr</extension>						\
		    </format>									\
		    <input bits=\"8-32\"/>							\
		    <input float=\"true\"/>							\
		    <parameters>								\
		      <selection name=\"Output format\" argument=\"-f %VALUE\" default=\"wav\">	\
			<option alias=\"Microsoft Wave File\">wav</option>			\
			<option alias=\"Apple Audio File\">aiff</option>			\
			<option alias=\"Apple Core Audio File\">caf</option>			\
			<option alias=\"Sony Media Wave64 File\">w64</option>			\
			<option alias=\"RIFF 64 Audio File\">rf64</option>			\
			<option alias=\"Sun Audio File\">au</option>				\
			<option alias=\"Creative Voice File\">voc</option>			\
			<option alias=\"Amiga Audio File\">iff</option>				\
			<option alias=\"IRCAM Sound File\">sf</option>				\
			<option alias=\"Paris Audio File\">paf</option>				\
			<option alias=\"Portable Voice Format\">pvf</option>			\
			<option alias=\"Psion WVE File\">wve</option>				\
			<option alias=\"HMM Toolkit Format\">htk</option>			\
			<option alias=\"Audio Visual Research Format\">avr</option>		\
		      </selection>								\
		    </parameters>								\
		  </component>									\
												\
		";

#ifdef __APPLE__
		componentSpecs.Replace("default=\"wav\"", "default=\"aiff\"");
#endif

		componentSpecs.Replace("%VERSION%", String("v").Append(String(ex_sf_version_string()).Replace("libsndfile-", NIL)));
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

namespace BoCA
{
	sf_count_t	 sf_vio_get_filelen_callback(void *);
	sf_count_t	 sf_vio_seek_callback(sf_count_t, int, void *);
	sf_count_t	 sf_vio_read_callback(void *, sf_count_t, void *);
	sf_count_t	 sf_vio_write_callback(const void *, sf_count_t, void *);
	sf_count_t	 sf_vio_tell_callback(void *);
};

BoCA::EncoderSndFile::EncoderSndFile()
{
	configLayer = NIL;
	config	    = NIL;

	fileFormat  = 0;

	sndf	    = NIL;
}

BoCA::EncoderSndFile::~EncoderSndFile()
{
	if (config != NIL) Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderSndFile::Activate()
{
	const Format	&format = track.GetFormat();

	/* Get configuration.
	 */
	config = Config::Copy(GetConfiguration());

	ConvertArguments(config);

	/* Get selected file format.
	 */
#ifdef __APPLE__
	fileFormat = config->GetIntValue(ConfigureSndFile::ConfigID, "Format", SF_FORMAT_AIFF);
#else
	fileFormat = config->GetIntValue(ConfigureSndFile::ConfigID, "Format", SF_FORMAT_WAV);
#endif

	/* Fill format info structure.
	 */
	SF_INFO	 sinfo;

	memset(&sinfo, 0, sizeof(SF_INFO));

	sinfo.format	 = fileFormat | config->GetIntValue(ConfigureSndFile::ConfigID, "SubFormat", 0);
	sinfo.samplerate = format.rate;
	sinfo.channels	 = format.channels;

	if (config->GetIntValue(ConfigureSndFile::ConfigID, "SubFormat", 0) == 0) sinfo.format |= SelectBestSubFormat(format, fileFormat);

	/* Check if selected format is supported.
	 */
	if (!ex_sf_format_check(&sinfo))
	{
		I18n	*i18n = I18n::Get();

		i18n->SetContext("Encoders::SndFile::Errors");

		if (config->GetIntValue(ConfigureSndFile::ConfigID, "SubFormat", 0) == 0) errorString = i18n->TranslateString("Could not find suitable audio format.");
		else									  errorString = i18n->TranslateString("Unable to use selected audio format.");

		errorState = True;

		return False;
	}

	/* Open sndfile handle.
	 */
	SF_VIRTUAL_IO	 vio;

	vio.get_filelen	= sf_vio_get_filelen_callback;
	vio.seek	= sf_vio_seek_callback;
	vio.read	= sf_vio_read_callback;
	vio.write	= sf_vio_write_callback;
	vio.tell	= sf_vio_tell_callback;

	sndf = ex_sf_open_virtual(&vio, SFM_WRITE, &sinfo, this);

	if (sndf == NIL) return False;

	ex_sf_command(sndf, SFC_SET_SCALE_INT_FLOAT_WRITE, NIL, SF_TRUE);

	/* Set metadata.
	 */
	if (fileFormat != SF_FORMAT_WAV && fileFormat != SF_FORMAT_RF64 && fileFormat != SF_FORMAT_W64)
	{
		Info	 info = track.GetInfo();

		if (info.HasBasicInfo())
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

	/* Write metadata.
	 */
	const Info	&info = track.GetInfo();

	/* Write RIFF and CART tags to Wave and RF64 files.
	 */
	if (fileFormat == SF_FORMAT_WAV || fileFormat == SF_FORMAT_RF64)
	{
		/* Write RIFF tag if requested.
		 */
		if (config->GetIntValue("Tags", "EnableRIFFINFOTag", True) && info.HasBasicInfo())
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("riff-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 tagBuffer;

				tagger->SetConfiguration(config);
				tagger->RenderBuffer(tagBuffer, track);

				driver->WriteData(tagBuffer, tagBuffer.Size());

				boca.DeleteComponent(tagger);
			}
		}

		/* Write CART tag if requested.
		 */
		if (config->GetIntValue("Tags", "EnableRIFFCartTag", True) && info.HasBasicInfo())
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("cart-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 tagBuffer;

				tagger->SetConfiguration(config);
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

	/* Write RIFF tags to Wave64 files.
	 */
	if (fileFormat == SF_FORMAT_W64)
	{
		/* Write RIFF tag if requested.
		 */
		if (config->GetIntValue("Tags", "EnableRIFFINFOTag", True) && info.HasBasicInfo())
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("riff-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 tagBuffer;

				tagger->SetConfiguration(config);
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

	/* Write ID3v2 tags to Wave and AIFF files.
	 */
	if (fileFormat == SF_FORMAT_WAV || fileFormat == SF_FORMAT_AIFF)
	{
		/* Write ID3v2 tag if requested.
		 */
		if (config->GetIntValue("Tags", "EnableID3v2", True) && (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->SetConfiguration(config);
				tagger->RenderBuffer(id3Buffer, track);

				if (fileFormat == SF_FORMAT_WAV) driver->WriteData((unsigned char *) "id3 ", 4);
				else				 driver->WriteData((unsigned char *) "ID3 ", 4);

				Int	 size = id3Buffer.Size();

				if ((fileFormat == SF_FORMAT_WAV  && endianness == EndianLittle) ||
				    (fileFormat == SF_FORMAT_AIFF && endianness == EndianBig)) for (Int i = 0; i <= 3; i++) driver->WriteData(((unsigned char *) &size) + i, 1);
				else							       for (Int i = 3; i >= 0; i--) driver->WriteData(((unsigned char *) &size) + i, 1);

				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}

		/* Write file size to header.
		 */
		Int64	 fileSize = driver->GetSize() - 8;

		if (fileSize >= (Int64(1) << 32)) fileSize = -1;

		driver->Seek(4);

		if (fileFormat == SF_FORMAT_WAV)
		{
			if (endianness == EndianLittle)	for (Int i = 0; i <= 3; i++) driver->WriteData(((unsigned char *) &fileSize) + i, 1);
			else				for (Int i = 7; i >= 4; i--) driver->WriteData(((unsigned char *) &fileSize) + i, 1);
		}
		else
		{
			if (endianness == EndianLittle)	for (Int i = 3; i >= 0; i--) driver->WriteData(((unsigned char *) &fileSize) + i, 1);
			else				for (Int i = 4; i <= 7; i++) driver->WriteData(((unsigned char *) &fileSize) + i, 1);
		}

		driver->Seek(driver->GetSize());
	}

	return True;
}

Int BoCA::EncoderSndFile::WriteData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format = track.GetFormat();

	/* Reorder channels.
	 */
	if (fileFormat == SF_FORMAT_AIFF ||
	    fileFormat == SF_FORMAT_CAF)
	{
		if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_1, Channel::AIFF_5_1);
	}

	/* Hand data to libsndfile.
	 */
	int	 bytes	= 0;

	if (format.bits == 8)
	{
		Buffer<short>	 buffer(data.Size());

		for (Int i = 0; i < data.Size(); i++) buffer[i] = ((signed char *) (UnsignedByte *) data)[i] << 8;

		bytes = ex_sf_write_short(sndf, buffer, data.Size());
	}
	else if	(format.bits == 16)
	{
		bytes = ex_sf_write_short(sndf, (short *) (UnsignedByte *) data, data.Size() / 2) * 2;
	}
	else if (format.bits == 24)
	{
		Buffer<int>	 buffer(data.Size() / 3);

		for (Int i = 0; i < data.Size() / 3; i++)
		{
			if (endianness == EndianLittle)	{ buffer[i] = (int) (data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8); }
			else				{ buffer[i] = (int) (data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8); }
		}

		bytes = ex_sf_write_int(sndf, buffer, data.Size() / 3) * 3;
	}
	else if (format.bits == 32 && !format.fp)
	{
		bytes = ex_sf_write_int(sndf, (int *) (UnsignedByte *) data, data.Size() / 4) * 4;
	}
	else if (format.bits == 32 && format.fp)
	{
		bytes = ex_sf_write_float(sndf, (float *) (UnsignedByte *) data, data.Size() / 4) * 4;
	}

	return bytes;
}

Int BoCA::EncoderSndFile::SelectBestSubFormat(const Format &format, Int fileFormat)
{
	static Int	 formats8Bit[]	= { SF_FORMAT_PCM_S8, SF_FORMAT_PCM_U8, SF_FORMAT_PCM_16, SF_FORMAT_PCM_24, SF_FORMAT_PCM_32, SF_FORMAT_FLOAT, SF_FORMAT_DOUBLE, SF_FORMAT_ULAW, SF_FORMAT_ALAW, NIL };
	static Int	 formats16Bit[]	= { SF_FORMAT_PCM_16, SF_FORMAT_PCM_24, SF_FORMAT_PCM_32, SF_FORMAT_FLOAT, SF_FORMAT_DOUBLE, SF_FORMAT_PCM_S8, SF_FORMAT_PCM_U8, SF_FORMAT_ULAW, SF_FORMAT_ALAW, NIL };
	static Int	 formats24Bit[]	= { SF_FORMAT_PCM_24, SF_FORMAT_PCM_32, SF_FORMAT_FLOAT, SF_FORMAT_DOUBLE, SF_FORMAT_PCM_16, SF_FORMAT_PCM_S8, SF_FORMAT_PCM_U8, SF_FORMAT_ULAW, SF_FORMAT_ALAW, NIL };
	static Int	 formats32Bit[]	= { SF_FORMAT_PCM_32, SF_FORMAT_FLOAT, SF_FORMAT_DOUBLE, SF_FORMAT_PCM_24, SF_FORMAT_PCM_16, SF_FORMAT_PCM_S8, SF_FORMAT_PCM_U8, SF_FORMAT_ULAW, SF_FORMAT_ALAW, NIL };
	static Int	 formatsFloat[] = { SF_FORMAT_FLOAT, SF_FORMAT_DOUBLE, SF_FORMAT_PCM_32, SF_FORMAT_PCM_24, SF_FORMAT_PCM_16, SF_FORMAT_PCM_S8, SF_FORMAT_PCM_U8, SF_FORMAT_ULAW, SF_FORMAT_ALAW, NIL };

	Int	*formats = NIL;

	if	( format.bits <=  8) formats = formats8Bit;
	else if ( format.bits <= 16) formats = formats16Bit;
	else if ( format.bits <= 24) formats = formats24Bit;
	else if	(!format.fp	   ) formats = formats32Bit;
	else			     formats = formatsFloat;

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

Bool BoCA::EncoderSndFile::SetOutputFormat(Int n)
{
	Config	*config = Config::Get();

	SF_FORMAT_INFO	 format_info;

	switch (n)
	{
		case  0: format_info.format = SF_FORMAT_WAV;   break;
		case  1: format_info.format = SF_FORMAT_AIFF;  break;
		case  2: format_info.format = SF_FORMAT_CAF;   break;
		case  3: format_info.format = SF_FORMAT_W64;   break;
		case  4: format_info.format = SF_FORMAT_RF64;  break;
		case  5: format_info.format = SF_FORMAT_AU;    break;
		case  6: format_info.format = SF_FORMAT_VOC;   break;
		case  7: format_info.format = SF_FORMAT_SVX;   break;
		case  9: format_info.format = SF_FORMAT_IRCAM; break;
		case  8: format_info.format = SF_FORMAT_PAF;   break;
		case 10: format_info.format = SF_FORMAT_PVF;   break;
		case 12: format_info.format = SF_FORMAT_WVE;   break;
		case 11: format_info.format = SF_FORMAT_HTK;   break;
		case 13: format_info.format = SF_FORMAT_AVR;   break;
		default:				       return False;
	}

	/* Check if output format is already set.
	 */
#ifdef __APPLE__
	if (config->GetIntValue(ConfigureSndFile::ConfigID, "Format", SF_FORMAT_AIFF) == format_info.format) return True;
#else
	if (config->GetIntValue(ConfigureSndFile::ConfigID, "Format", SF_FORMAT_WAV)  == format_info.format) return True;
#endif

	/* Set new output format.
	 */
	config->SetIntValue(ConfigureSndFile::ConfigID, "Format", format_info.format);
	config->SetIntValue(ConfigureSndFile::ConfigID, "SubFormat", 0);

	return True;
}

String BoCA::EncoderSndFile::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	SF_FORMAT_INFO	 format_info;

#ifdef __APPLE__
	format_info.format = config->GetIntValue(ConfigureSndFile::ConfigID, "Format", SF_FORMAT_AIFF);
#else
	format_info.format = config->GetIntValue(ConfigureSndFile::ConfigID, "Format", SF_FORMAT_WAV);
#endif

	ex_sf_command(NIL, SFC_GET_FORMAT_INFO, &format_info, sizeof(format_info));

	return format_info.extension;
}

Bool BoCA::EncoderSndFile::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "sndfile-enc";

	/* Get command line settings.
	 */
	String	 format = "wav";

	if (config->GetIntValue(encoderID, "Set Output format", False)) format = config->GetStringValue(encoderID, "Output format", format).ToLower();

	/* Set configuration values.
	 */
#ifdef __APPLE__
	Int	 outputFormat = SF_FORMAT_AIFF;
#else
	Int	 outputFormat = SF_FORMAT_WAV;
#endif

	if	(format == "wav" ) outputFormat = SF_FORMAT_WAV;
	else if (format == "aiff") outputFormat = SF_FORMAT_AIFF;
	else if (format == "caf" ) outputFormat = SF_FORMAT_CAF;
	else if (format == "w64" ) outputFormat = SF_FORMAT_W64;
	else if (format == "rf64") outputFormat = SF_FORMAT_RF64;
	else if (format == "au"	 ) outputFormat = SF_FORMAT_AU;
	else if (format == "voc" ) outputFormat = SF_FORMAT_VOC;
	else if (format == "iff" ) outputFormat = SF_FORMAT_SVX;
	else if (format == "sf"	 ) outputFormat = SF_FORMAT_IRCAM;
	else if (format == "paf" ) outputFormat = SF_FORMAT_PAF;
	else if (format == "pvf" ) outputFormat = SF_FORMAT_PVF;
	else if (format == "wve" ) outputFormat = SF_FORMAT_WVE;
	else if (format == "htk" ) outputFormat = SF_FORMAT_HTK;
	else if (format == "avr" ) outputFormat = SF_FORMAT_AVR;

	config->SetIntValue(ConfigureSndFile::ConfigID, "Format", outputFormat);
	config->SetIntValue(ConfigureSndFile::ConfigID, "SubFormat", 0);

	return True;
}

ConfigLayer *BoCA::EncoderSndFile::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureSndFile();

	return configLayer;
}

sf_count_t BoCA::sf_vio_get_filelen_callback(void *user_data)
{
	EncoderSndFile	*filter = (EncoderSndFile *) user_data;

	return filter->driver->GetSize();
}

sf_count_t BoCA::sf_vio_seek_callback(sf_count_t offset, int whence, void *user_data)
{
	EncoderSndFile	*filter = (EncoderSndFile *) user_data;

	if	(whence == SEEK_CUR) filter->driver->Seek(filter->driver->GetPos()  + offset);
	else if	(whence == SEEK_SET) filter->driver->Seek(			      offset);
	else if	(whence == SEEK_END) filter->driver->Seek(filter->driver->GetSize() + offset);

	return filter->driver->GetPos();
}

sf_count_t BoCA::sf_vio_read_callback(void *ptr, sf_count_t count, void *user_data)
{
	EncoderSndFile	*filter = (EncoderSndFile *) user_data;

	return filter->driver->ReadData((UnsignedByte *) ptr, count);
}

sf_count_t BoCA::sf_vio_write_callback(const void * ptr, sf_count_t count, void *user_data)
{
	EncoderSndFile	*filter = (EncoderSndFile *) user_data;

	return filter->driver->WriteData((UnsignedByte *) ptr, count);
}

sf_count_t BoCA::sf_vio_tell_callback(void *user_data)
{
	EncoderSndFile	*filter = (EncoderSndFile *) user_data;

	return filter->driver->GetPos();
}
