 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "flac.h"

using namespace smooth::IO;

const String &BoCA::TaggerFLAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (flacdll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>FLAC Audio Tagger</name>		\
		    <version>1.0</version>			\
		    <id>flac-tag</id>				\
		    <type>tagger</type>				\
		    <require>vorbis-tag</require>		\
		    <format>					\
		      <name>FLAC Audio Files</name>		\
		      <extension>flac</extension>		\
		    </format>					\
								\
		";

		if (*ex_FLAC_API_SUPPORTS_OGG_FLAC == 1)
		{
			componentSpecs.Append("			\
								\
			    <format>				\
			      <name>Ogg FLAC Files</name>	\
			      <extension>oga</extension>	\
			    </format>				\
								\
			");
		}

		componentSpecs.Append("				\
								\
		    <tagspec>					\
		      <name>FLAC Metadata</name>		\
		      <coverart supported=\"true\"/>		\
		      <encodings>				\
			<encoding>UTF-8</encoding>		\
		      </encodings>				\
		    </tagspec>					\
		  </component>					\
								\
		");
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadFLACDLL();
}

Void smooth::DetachDLL()
{
	FreeFLACDLL();
}

BoCA::TaggerFLAC::TaggerFLAC()
{
}

BoCA::TaggerFLAC::~TaggerFLAC()
{
}

Error BoCA::TaggerFLAC::RenderStreamInfo(const String &streamURI, const Track &track)
{
	/* Render works like update for FLAC files.
	 */
	return UpdateStreamInfo(streamURI, track);
}

Error BoCA::TaggerFLAC::UpdateStreamInfo(const String &streamURI, const Track &track)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	String	 fileType = in.InputString(4);

	if (fileType != "fLaC" && fileType != "OggS")		       return Error();
	if (fileType == "OggS" && *ex_FLAC_API_SUPPORTS_OGG_FLAC == 0) return Error();

	in.Close();

	const Config	*config = GetConfiguration();

	FLAC__Metadata_Chain	*chain = ex_FLAC__metadata_chain_new();

	if (String::IsUnicode(streamURI))
	{
		File(streamURI).Copy(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".tag"));

		FLAC__bool	 success = false;

		if	(fileType == "fLaC") success = ex_FLAC__metadata_chain_read(chain, Utilities::GetNonUnicodeTempFileName(streamURI).Append(".tag"));
		else if (fileType == "OggS") success = ex_FLAC__metadata_chain_read_ogg(chain, Utilities::GetNonUnicodeTempFileName(streamURI).Append(".tag"));

		if (!success)
		{
			File(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".tag")).Delete();

			ex_FLAC__metadata_chain_delete(chain);

			return Error();
		}
	}
	else
	{
		FLAC__bool	 success = false;

		if	(fileType == "fLaC") success = ex_FLAC__metadata_chain_read(chain, streamURI);
		else if (fileType == "OggS") success = ex_FLAC__metadata_chain_read_ogg(chain, streamURI);

		if (!success)
		{
			ex_FLAC__metadata_chain_delete(chain);

			return Error();
		}
	}

	FLAC__Metadata_Iterator	*iterator = ex_FLAC__metadata_iterator_new();

	ex_FLAC__metadata_iterator_init(iterator, chain);

	/* Delete existing metadata first.
	 */
	do
	{
		FLAC__MetadataType	 type = ex_FLAC__metadata_iterator_get_block_type(iterator);

		if	(type == FLAC__METADATA_TYPE_VORBIS_COMMENT) ex_FLAC__metadata_iterator_delete_block(iterator, true);
		else if (type == FLAC__METADATA_TYPE_PICTURE)	     ex_FLAC__metadata_iterator_delete_block(iterator, false);
	}
	while (ex_FLAC__metadata_iterator_next(iterator));

	/* Now write updated metadata.
	 */
	const Info		&info = track.GetInfo();
	Buffer<unsigned char>	 vcBuffer;

	if (((track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)) ||
	     (info.artist != NIL || info.title != NIL)) && config->GetIntValue("Tags", "EnableFLACMetadata", True))
	{
		FLAC__StreamMetadata	*vorbiscomment = ex_FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT);

		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			/* Disable writing cover art to Vorbis comment tags for FLAC files.
			 */
			Config	*taggerConfig = Config::Copy(config);

			taggerConfig->SetIntValue("Tags", "CoverArtWriteToVorbisComment", False);

			tagger->SetConfiguration(taggerConfig);
			tagger->SetVendorString(*ex_FLAC__VENDOR_STRING);

			tagger->RenderBuffer(vcBuffer, track);

			boca.DeleteComponent(tagger);

			Config::Free(taggerConfig);
		}

		/* Process output comment tag and add it to FLAC metadata.
		 */
		InStream	in(STREAM_BUFFER, vcBuffer, vcBuffer.Size());

		in.RelSeek(in.InputNumber(4));

		Int	 numComments = in.InputNumber(4);

		ex_FLAC__metadata_object_vorbiscomment_resize_comments(vorbiscomment, numComments);

		for (Int i = 0; i < numComments; i++)
		{
			vorbiscomment->data.vorbis_comment.comments[i].length = in.InputNumber(4);
			vorbiscomment->data.vorbis_comment.comments[i].entry = vcBuffer + in.GetPos();

			in.RelSeek(vorbiscomment->data.vorbis_comment.comments[i].length);
		}

		vorbiscomment->length = vcBuffer.Size();

		ex_FLAC__metadata_iterator_insert_block_after(iterator, vorbiscomment);
	}

	if (config->GetIntValue("Tags", "CoverArtWriteToTags", True) && config->GetIntValue("Tags", "CoverArtWriteToFLACMetadata", True))
	{
		for (Int i = 0; i < track.pictures.Length(); i++)
		{
			FLAC__StreamMetadata	*picture = ex_FLAC__metadata_object_new(FLAC__METADATA_TYPE_PICTURE);
			const Picture		&picInfo = track.pictures.GetNth(i);

			if (picInfo.mime != NIL)	ex_FLAC__metadata_object_picture_set_mime_type(picture, picInfo.mime, true);
			if (picInfo.description != NIL) ex_FLAC__metadata_object_picture_set_description(picture, (FLAC__byte *) picInfo.description.Trim().ConvertTo("UTF-8"), true);

			ex_FLAC__metadata_object_picture_set_data(picture, const_cast<UnsignedByte *>((const UnsignedByte *) picInfo.data), picInfo.data.Size(), true);

			picture->data.picture.type = (FLAC__StreamMetadata_Picture_Type) picInfo.type;

			ex_FLAC__metadata_iterator_insert_block_after(iterator, picture);
		}
	}

	ex_FLAC__metadata_iterator_delete(iterator);

	/* Adjust padding and write changes.
	 */
	ex_FLAC__metadata_chain_sort_padding(chain);

	if (!ex_FLAC__metadata_chain_write(chain, true, false)) { errorState = True; }

	ex_FLAC__metadata_chain_delete(chain);

	if (String::IsUnicode(streamURI))
	{
		if (!errorState)
		{
			File(streamURI).Delete();
			File(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".tag")).Move(streamURI);
		}
		else
		{
			File(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".tag")).Delete();
		}
	}

	if (errorState)	return Error();
	else		return Success();
}
