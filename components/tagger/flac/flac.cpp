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

#include <smooth.h>
#include <smooth/dll.h>

#include "flac.h"

using namespace smooth::IO;

const String &BoCA::TaggerFLAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (flacdll != NIL)
	{
		componentSpecs = "					\
									\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>		\
		  <component>						\
		    <name>FLAC Audio Tagger</name>			\
		    <version>1.0</version>				\
		    <id>flac-tag</id>					\
		    <type>tagger</type>					\
		    <require>vorbis-tag</require>			\
		    <format>						\
		      <name>FLAC Audio Files</name>			\
		      <extension>flac</extension>			\
		    </format>						\
									\
		";

		if (*ex_FLAC_API_SUPPORTS_OGG_FLAC == 1)
		{
			componentSpecs.Append("				\
									\
			    <format>					\
			      <name>Ogg FLAC Files</name>		\
			      <extension>oga</extension>		\
			    </format>					\
									\
			");
		}

		componentSpecs.Append("					\
									\
		    <tagspec>						\
		      <name>FLAC Metadata</name>			\
		      <coverart supported=\"true\"/>			\
		      <prependzero allowed=\"true\" default=\"true\"/>	\
		      <encodings>					\
			<encoding>UTF-8</encoding>			\
		      </encodings>					\
		    </tagspec>						\
		  </component>						\
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

namespace BoCA
{
	size_t		 FLACIOCallbackRead(void *, size_t, size_t, FLAC__IOHandle);
	size_t		 FLACIOCallbackWrite(const void *, size_t, size_t, FLAC__IOHandle);
	int		 FLACIOCallbackSeek(FLAC__IOHandle, FLAC__int64, int);
	FLAC__int64	 FLACIOCallbackTell(FLAC__IOHandle);
	int		 FLACIOCallbackEof(FLAC__IOHandle);
};

const String	 BoCA::TaggerFLAC::ConfigID = "Tags";

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
	InStream	 in(STREAM_FILE, streamURI, IS_READ | IS_WRITE);

	String	 fileType = in.InputString(4);

	if (fileType != "fLaC" && fileType != "OggS")		       return Error();
	if (fileType == "OggS" && *ex_FLAC_API_SUPPORTS_OGG_FLAC == 0) return Error();

	/* Get configuration.
	 */
	const Config	*currentConfig	     = GetConfiguration();

	Bool		 enableFLACMetadata  = currentConfig->GetIntValue(ConfigID, "EnableFLACMetadata", True);

	Bool		 prependZero	     = currentConfig->GetIntValue(ConfigID, "TrackPrependZeroFLACMetadata", True);

	Bool		 writeChapters	     = currentConfig->GetIntValue(ConfigID, "WriteChapters", True);

	Bool		 coverArtWriteToTags = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToTags", True);
	Bool		 coverArtWriteToFLAC = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToFLACMetadata", True);

	/* Set up callbacks.
	 */
	FLAC__IOCallbacks	 callbacks;

	callbacks.read	= FLACIOCallbackRead;
	callbacks.write	= FLACIOCallbackWrite;
	callbacks.seek	= FLACIOCallbackSeek;
	callbacks.tell	= FLACIOCallbackTell;
	callbacks.eof	= FLACIOCallbackEof;
	callbacks.close	= NIL;

	FLAC__bool		 success = false;
	FLAC__Metadata_Chain	*chain	 = ex_FLAC__metadata_chain_new();

	if	(fileType == "fLaC") success = ex_FLAC__metadata_chain_read_with_callbacks(chain, (FLAC__IOHandle) &in, callbacks);
	else if (fileType == "OggS") success = ex_FLAC__metadata_chain_read_ogg_with_callbacks(chain, (FLAC__IOHandle) &in, callbacks);

	if (!success)
	{
		ex_FLAC__metadata_chain_delete(chain);

		return Error();
	}

	FLAC__Metadata_Iterator	*iterator = ex_FLAC__metadata_iterator_new();

	ex_FLAC__metadata_iterator_init(iterator, chain);

	/* Delete existing metadata first.
	 */
	Int64	 pictureSizeBefore = 0;

	do
	{
		FLAC__MetadataType	 type = ex_FLAC__metadata_iterator_get_block_type(iterator);

		if	(type == FLAC__METADATA_TYPE_VORBIS_COMMENT)										    ex_FLAC__metadata_iterator_delete_block(iterator, true);
		else if (type == FLAC__METADATA_TYPE_PICTURE)	     { pictureSizeBefore += ex_FLAC__metadata_iterator_get_block(iterator)->length; ex_FLAC__metadata_iterator_delete_block(iterator, false); }
	}
	while (ex_FLAC__metadata_iterator_next(iterator));

	/* Now write updated metadata.
	 */
	const Info		&info = track.GetInfo();
	Buffer<unsigned char>	 vcBuffer;

	if (enableFLACMetadata && (info.HasBasicInfo() || (track.tracks.Length() > 0 && writeChapters)))
	{
		FLAC__StreamMetadata	*vorbiscomment = ex_FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT);

		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			/* Disable writing cover art to Vorbis comment tags for FLAC files.
			 */
			Config	*taggerConfig = Config::Copy(currentConfig);

			taggerConfig->SetIntValue(ConfigID, "CoverArtWriteToVorbisComment", False);
			taggerConfig->SetIntValue(ConfigID, "TrackPrependZeroVorbisComment", prependZero);

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

		for (Int i = 0; i < numComments; i++)
		{
			FLAC__StreamMetadata_VorbisComment_Entry	 entry;

			entry.length = in.InputNumber(4);
			entry.entry  = vcBuffer + in.GetPos();

			ex_FLAC__metadata_object_vorbiscomment_append_comment(vorbiscomment, entry, true);

			in.RelSeek(entry.length);
		}

		vorbiscomment->length = vcBuffer.Size();

		ex_FLAC__metadata_iterator_insert_block_after(iterator, vorbiscomment);
	}

	Int64	 pictureSizeAfter = 0;

	if (coverArtWriteToTags && coverArtWriteToFLAC)
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

			pictureSizeAfter += picture->length;
		}
	}

	ex_FLAC__metadata_iterator_delete(iterator);

	/* Adjust padding and write changes.
	 */
	ex_FLAC__metadata_chain_sort_padding(chain);

	if (!ex_FLAC__metadata_chain_check_if_tempfile_needed(chain) && pictureSizeBefore - pictureSizeAfter <= 4096)
	{
		OutStream	 out(STREAM_STREAM, &in);

		if (!ex_FLAC__metadata_chain_write_with_callbacks(chain, true, (FLAC__IOHandle) &out, callbacks)) { errorState = True; }
	}
	else
	{
		OutStream	 out(STREAM_FILE, streamURI.Append(".temp"), OS_REPLACE);

		if (!ex_FLAC__metadata_chain_write_with_callbacks_and_tempfile(chain, pictureSizeBefore - pictureSizeAfter <= 4096, (FLAC__IOHandle) &in, callbacks, (FLAC__IOHandle) &out, callbacks)) { errorState = True; }

		out.Close();
		in.Close();

		if (!errorState)
		{
			File(streamURI).Delete();
			File(streamURI.Append(".temp")).Move(streamURI);
		}
	}

	ex_FLAC__metadata_chain_delete(chain);

	if (errorState)	return Error();
	else		return Success();
}

size_t BoCA::FLACIOCallbackRead(void *ptr, size_t size, size_t nmemb, FLAC__IOHandle handle)
{
	InStream	*stream = (InStream *) handle;

	return stream->InputData(ptr, size * nmemb) / size;
}

size_t BoCA::FLACIOCallbackWrite(const void *ptr, size_t size, size_t nmemb, FLAC__IOHandle handle)
{
	OutStream	*stream = (OutStream *) handle;

	if (stream->OutputData(ptr, size * nmemb)) return nmemb;
	else					   return 0;
}

int BoCA::FLACIOCallbackSeek(FLAC__IOHandle handle, FLAC__int64 offset, int whence)
{
	Stream	*stream = (Stream *) handle;

	if	(whence == SEEK_CUR) stream->Seek(stream->GetPos() + offset);
	else if	(whence == SEEK_SET) stream->Seek(		     offset);
	else if	(whence == SEEK_END) stream->Seek(stream->Size()   + offset);

	return 0;
}

FLAC__int64 BoCA::FLACIOCallbackTell(FLAC__IOHandle handle)
{
	Stream	*stream = (Stream *) handle;

	return stream->GetPos();
}

FLAC__bool BoCA::FLACIOCallbackEof(FLAC__IOHandle handle)
{
	Stream	*stream = (Stream *) handle;

	return stream->GetPos() >= stream->Size();
}
