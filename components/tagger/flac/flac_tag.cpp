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

#include "flac_tag.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::FLACTag::GetComponentSpecs()
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
		    <format>					\
		      <name>FLAC Audio Files</name>		\
		      <extension>flac</extension>		\
		    </format>					\
		    <tagformat>					\
		      <name>FLAC Metadata</name>		\
		      <coverart supported=\"true\"/>		\
		      <encodings>				\
			<encoding>UTF-8</encoding>		\
		      </encodings>				\
		    </tagformat>				\
		  </component>					\
								\
		";
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

BoCA::FLACTag::FLACTag()
{
}

BoCA::FLACTag::~FLACTag()
{
}

Error BoCA::FLACTag::UpdateStreamInfo(const String &streamURI, const Track &track)
{
	Config	*config = Config::Get();

	FLAC__Metadata_Chain	*chain = ex_FLAC__metadata_chain_new();

	if (String::IsUnicode(streamURI))
	{
		File(streamURI).Copy(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".tag"));

		ex_FLAC__metadata_chain_read(chain, Utilities::GetNonUnicodeTempFileName(streamURI).Append(".tag"));
	}
	else
	{
		ex_FLAC__metadata_chain_read(chain, streamURI);
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

	if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableVorbisComment", True))
	{
		FLAC__StreamMetadata	*vorbiscomment = ex_FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT);

		/* Disable writing cover art to Vorbis comment tags for FLAC files.
		 */
		Bool	 writeVorbisCoverArt = config->GetIntValue("Tags", "CoverArtWriteToVorbisComment", False);

		if (writeVorbisCoverArt) config->SetIntValue("Tags", "CoverArtWriteToVorbisComment", False);

		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			tagger->SetVendorString(*ex_FLAC__VENDOR_STRING);

			tagger->RenderBuffer(vcBuffer, track);

			boca.DeleteComponent(tagger);
		}

		if (writeVorbisCoverArt) config->SetIntValue("Tags", "CoverArtWriteToVorbisComment", True);

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

	if (config->GetIntValue("Tags", "CoverArtWriteToTags", True))
	{
		for (Int i = 0; i < track.pictures.Length(); i++)
		{
			FLAC__StreamMetadata	*picture = ex_FLAC__metadata_object_new(FLAC__METADATA_TYPE_PICTURE);
			const Picture		&picInfo = track.pictures.GetNth(i);

			if (picInfo.description != NIL) ex_FLAC__metadata_object_picture_set_description(picture, (FLAC__byte *) picInfo.description.ConvertTo("UTF-8"), true);

			ex_FLAC__metadata_object_picture_set_mime_type(picture, picInfo.mime, true);
			ex_FLAC__metadata_object_picture_set_data(picture, picInfo.data, picInfo.data.Size(), true);

			picture->data.picture.type = (FLAC__StreamMetadata_Picture_Type) picInfo.type;

			ex_FLAC__metadata_iterator_insert_block_after(iterator, picture);
		}
	}

	ex_FLAC__metadata_iterator_delete(iterator);

	/* Adjust padding and write changes.
	 */
	ex_FLAC__metadata_chain_sort_padding(chain);
	ex_FLAC__metadata_chain_write(chain, true, false);

	ex_FLAC__metadata_chain_delete(chain);

	if (String::IsUnicode(streamURI))
	{
		File(streamURI).Delete();
		File(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".tag")).Move(streamURI);
	}

	return Success();
}
