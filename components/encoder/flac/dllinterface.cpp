 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

using namespace BoCA;

FLAC_API_SUPPORTS_OGG_FLAC_TYPE					 ex_FLAC_API_SUPPORTS_OGG_FLAC							= NIL;

FLAC__STREAM_ENCODER_NEW					 ex_FLAC__stream_encoder_new							= NIL;
FLAC__STREAM_ENCODER_DELETE					 ex_FLAC__stream_encoder_delete							= NIL;
FLAC__STREAM_ENCODER_INIT_STREAM				 ex_FLAC__stream_encoder_init_stream						= NIL;
FLAC__STREAM_ENCODER_INIT_OGG_STREAM				 ex_FLAC__stream_encoder_init_ogg_stream					= NIL;
FLAC__STREAM_ENCODER_FINISH					 ex_FLAC__stream_encoder_finish							= NIL;
FLAC__STREAM_ENCODER_SET_CHANNELS				 ex_FLAC__stream_encoder_set_channels						= NIL;
FLAC__STREAM_ENCODER_SET_BITS_PER_SAMPLE			 ex_FLAC__stream_encoder_set_bits_per_sample					= NIL;
FLAC__STREAM_ENCODER_SET_SAMPLE_RATE				 ex_FLAC__stream_encoder_set_sample_rate					= NIL;
FLAC__STREAM_ENCODER_SET_STREAMABLE_SUBSET			 ex_FLAC__stream_encoder_set_streamable_subset					= NIL;
FLAC__STREAM_ENCODER_SET_DO_MID_SIDE_STEREO			 ex_FLAC__stream_encoder_set_do_mid_side_stereo					= NIL;
FLAC__STREAM_ENCODER_SET_LOOSE_MID_SIDE_STEREO			 ex_FLAC__stream_encoder_set_loose_mid_side_stereo				= NIL;
FLAC__STREAM_ENCODER_SET_BLOCKSIZE				 ex_FLAC__stream_encoder_set_blocksize						= NIL;
FLAC__STREAM_ENCODER_SET_MAX_LPC_ORDER				 ex_FLAC__stream_encoder_set_max_lpc_order					= NIL;
FLAC__STREAM_ENCODER_SET_QLP_COEFF_PRECISION			 ex_FLAC__stream_encoder_set_qlp_coeff_precision				= NIL;
FLAC__STREAM_ENCODER_SET_DO_QLP_COEFF_PREC_SEARCH		 ex_FLAC__stream_encoder_set_do_qlp_coeff_prec_search				= NIL;
FLAC__STREAM_ENCODER_SET_DO_EXHAUSTIVE_MODEL_SEARCH		 ex_FLAC__stream_encoder_set_do_exhaustive_model_search				= NIL;
FLAC__STREAM_ENCODER_SET_MIN_RESIDUAL_PARTITION_ORDER		 ex_FLAC__stream_encoder_set_min_residual_partition_order			= NIL;
FLAC__STREAM_ENCODER_SET_MAX_RESIDUAL_PARTITION_ORDER		 ex_FLAC__stream_encoder_set_max_residual_partition_order			= NIL;
FLAC__STREAM_ENCODER_SET_METADATA				 ex_FLAC__stream_encoder_set_metadata						= NIL;
FLAC__STREAM_ENCODER_SET_COMPRESSION_LEVEL			 ex_FLAC__stream_encoder_set_compression_level					= NIL;
FLAC__STREAM_ENCODER_SET_APODIZATION				 ex_FLAC__stream_encoder_set_apodization					= NIL;
FLAC__STREAM_ENCODER_SET_OGG_SERIAL_NUMBER			 ex_FLAC__stream_encoder_set_ogg_serial_number					= NIL;
FLAC__STREAM_ENCODER_PROCESS_INTERLEAVED			 ex_FLAC__stream_encoder_process_interleaved					= NIL;

FLAC__METADATA_OBJECT_NEW					 ex_FLAC__metadata_object_new							= NIL;
FLAC__METADATA_OBJECT_DELETE					 ex_FLAC__metadata_object_delete						= NIL;

FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE			 ex_FLAC__metadata_object_picture_set_mime_type					= NIL;
FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION			 ex_FLAC__metadata_object_picture_set_description				= NIL;
FLAC__METADATA_OBJECT_PICTURE_SET_DATA				 ex_FLAC__metadata_object_picture_set_data					= NIL;

FLAC__METADATA_OBJECT_CUESHEET_TRACK_NEW			 ex_FLAC__metadata_object_cuesheet_track_new					= NIL;
FLAC__METADATA_OBJECT_CUESHEET_TRACK_DELETE			 ex_FLAC__metadata_object_cuesheet_track_delete					= NIL;
FLAC__METADATA_OBJECT_CUESHEET_TRACK_RESIZE_INDICES		 ex_FLAC__metadata_object_cuesheet_track_resize_indices				= NIL;
FLAC__METADATA_OBJECT_CUESHEET_TRACK_INSERT_INDEX		 ex_FLAC__metadata_object_cuesheet_track_insert_index				= NIL;
FLAC__METADATA_OBJECT_CUESHEET_RESIZE_TRACKS			 ex_FLAC__metadata_object_cuesheet_resize_tracks				= NIL;
FLAC__METADATA_OBJECT_CUESHEET_INSERT_TRACK			 ex_FLAC__metadata_object_cuesheet_insert_track					= NIL;

FLAC__METADATA_OBJECT_VORBISCOMMENT_APPEND_COMMENT		 ex_FLAC__metadata_object_vorbiscomment_append_comment				= NIL;

FLAC__METADATA_OBJECT_SEEKTABLE_TEMPLATE_APPEND_SPACED_POINTS	 ex_FLAC__metadata_object_seektable_template_append_spaced_points		= NIL;
FLAC__METADATA_OBJECT_SEEKTABLE_TEMPLATE_SORT			 ex_FLAC__metadata_object_seektable_template_sort				= NIL;

FLAC__VERSION_STRING_TYPE					 ex_FLAC__VERSION_STRING							= NIL;
FLAC__VENDOR_STRING_TYPE					 ex_FLAC__VENDOR_STRING								= NIL;

DynamicLoader *flacdll	= NIL;

Bool LoadFLACDLL()
{
	flacdll = BoCA::Utilities::LoadCodecDLL("FLAC");

	if (flacdll == NIL) return False;

	ex_FLAC_API_SUPPORTS_OGG_FLAC						= (FLAC_API_SUPPORTS_OGG_FLAC_TYPE) flacdll->GetFunctionAddress("FLAC_API_SUPPORTS_OGG_FLAC");

	ex_FLAC__stream_encoder_new						= (FLAC__STREAM_ENCODER_NEW) flacdll->GetFunctionAddress("FLAC__stream_encoder_new");
	ex_FLAC__stream_encoder_delete						= (FLAC__STREAM_ENCODER_DELETE) flacdll->GetFunctionAddress("FLAC__stream_encoder_delete");
	ex_FLAC__stream_encoder_init_stream					= (FLAC__STREAM_ENCODER_INIT_STREAM) flacdll->GetFunctionAddress("FLAC__stream_encoder_init_stream");
	ex_FLAC__stream_encoder_init_ogg_stream					= (FLAC__STREAM_ENCODER_INIT_OGG_STREAM) flacdll->GetFunctionAddress("FLAC__stream_encoder_init_ogg_stream");
	ex_FLAC__stream_encoder_finish						= (FLAC__STREAM_ENCODER_FINISH) flacdll->GetFunctionAddress("FLAC__stream_encoder_finish");
	ex_FLAC__stream_encoder_set_channels					= (FLAC__STREAM_ENCODER_SET_CHANNELS) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_channels");
	ex_FLAC__stream_encoder_set_bits_per_sample				= (FLAC__STREAM_ENCODER_SET_BITS_PER_SAMPLE) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_bits_per_sample");
	ex_FLAC__stream_encoder_set_sample_rate					= (FLAC__STREAM_ENCODER_SET_SAMPLE_RATE) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_sample_rate");
	ex_FLAC__stream_encoder_set_streamable_subset				= (FLAC__STREAM_ENCODER_SET_STREAMABLE_SUBSET) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_streamable_subset");
	ex_FLAC__stream_encoder_set_do_mid_side_stereo				= (FLAC__STREAM_ENCODER_SET_DO_MID_SIDE_STEREO) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_do_mid_side_stereo");
	ex_FLAC__stream_encoder_set_loose_mid_side_stereo			= (FLAC__STREAM_ENCODER_SET_LOOSE_MID_SIDE_STEREO) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_loose_mid_side_stereo");
	ex_FLAC__stream_encoder_set_blocksize					= (FLAC__STREAM_ENCODER_SET_BLOCKSIZE) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_blocksize");
	ex_FLAC__stream_encoder_set_max_lpc_order				= (FLAC__STREAM_ENCODER_SET_MAX_LPC_ORDER) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_max_lpc_order");
	ex_FLAC__stream_encoder_set_qlp_coeff_precision				= (FLAC__STREAM_ENCODER_SET_QLP_COEFF_PRECISION) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_qlp_coeff_precision");
	ex_FLAC__stream_encoder_set_do_qlp_coeff_prec_search			= (FLAC__STREAM_ENCODER_SET_DO_QLP_COEFF_PREC_SEARCH) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_do_qlp_coeff_prec_search");
	ex_FLAC__stream_encoder_set_do_exhaustive_model_search			= (FLAC__STREAM_ENCODER_SET_DO_EXHAUSTIVE_MODEL_SEARCH) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_do_exhaustive_model_search");
	ex_FLAC__stream_encoder_set_min_residual_partition_order		= (FLAC__STREAM_ENCODER_SET_MIN_RESIDUAL_PARTITION_ORDER) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_min_residual_partition_order");
	ex_FLAC__stream_encoder_set_max_residual_partition_order		= (FLAC__STREAM_ENCODER_SET_MAX_RESIDUAL_PARTITION_ORDER) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_max_residual_partition_order");
	ex_FLAC__stream_encoder_set_metadata					= (FLAC__STREAM_ENCODER_SET_METADATA) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_metadata");
	ex_FLAC__stream_encoder_set_compression_level				= (FLAC__STREAM_ENCODER_SET_COMPRESSION_LEVEL) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_compression_level");
	ex_FLAC__stream_encoder_set_apodization					= (FLAC__STREAM_ENCODER_SET_APODIZATION) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_apodization");
	ex_FLAC__stream_encoder_set_ogg_serial_number				= (FLAC__STREAM_ENCODER_SET_OGG_SERIAL_NUMBER) flacdll->GetFunctionAddress("FLAC__stream_encoder_set_ogg_serial_number");
	ex_FLAC__stream_encoder_process_interleaved				= (FLAC__STREAM_ENCODER_PROCESS_INTERLEAVED) flacdll->GetFunctionAddress("FLAC__stream_encoder_process_interleaved");

	ex_FLAC__metadata_object_new						= (FLAC__METADATA_OBJECT_NEW) flacdll->GetFunctionAddress("FLAC__metadata_object_new");
	ex_FLAC__metadata_object_delete						= (FLAC__METADATA_OBJECT_DELETE) flacdll->GetFunctionAddress("FLAC__metadata_object_delete");

	ex_FLAC__metadata_object_picture_set_mime_type				= (FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE) flacdll->GetFunctionAddress("FLAC__metadata_object_picture_set_mime_type");
	ex_FLAC__metadata_object_picture_set_description			= (FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION) flacdll->GetFunctionAddress("FLAC__metadata_object_picture_set_description");
	ex_FLAC__metadata_object_picture_set_data				= (FLAC__METADATA_OBJECT_PICTURE_SET_DATA) flacdll->GetFunctionAddress("FLAC__metadata_object_picture_set_data");

	ex_FLAC__metadata_object_cuesheet_track_new				= (FLAC__METADATA_OBJECT_CUESHEET_TRACK_NEW) flacdll->GetFunctionAddress("FLAC__metadata_object_cuesheet_track_new");
	ex_FLAC__metadata_object_cuesheet_track_delete				= (FLAC__METADATA_OBJECT_CUESHEET_TRACK_DELETE) flacdll->GetFunctionAddress("FLAC__metadata_object_cuesheet_track_delete");
	ex_FLAC__metadata_object_cuesheet_track_resize_indices			= (FLAC__METADATA_OBJECT_CUESHEET_TRACK_RESIZE_INDICES) flacdll->GetFunctionAddress("FLAC__metadata_object_cuesheet_track_resize_indices");
	ex_FLAC__metadata_object_cuesheet_track_insert_index			= (FLAC__METADATA_OBJECT_CUESHEET_TRACK_INSERT_INDEX) flacdll->GetFunctionAddress("FLAC__metadata_object_cuesheet_track_insert_index");
	ex_FLAC__metadata_object_cuesheet_resize_tracks				= (FLAC__METADATA_OBJECT_CUESHEET_RESIZE_TRACKS) flacdll->GetFunctionAddress("FLAC__metadata_object_cuesheet_resize_tracks");
	ex_FLAC__metadata_object_cuesheet_insert_track				= (FLAC__METADATA_OBJECT_CUESHEET_INSERT_TRACK) flacdll->GetFunctionAddress("FLAC__metadata_object_cuesheet_insert_track");

	ex_FLAC__metadata_object_vorbiscomment_append_comment			= (FLAC__METADATA_OBJECT_VORBISCOMMENT_APPEND_COMMENT) flacdll->GetFunctionAddress("FLAC__metadata_object_vorbiscomment_append_comment");

	ex_FLAC__metadata_object_seektable_template_append_spaced_points	= (FLAC__METADATA_OBJECT_SEEKTABLE_TEMPLATE_APPEND_SPACED_POINTS) flacdll->GetFunctionAddress("FLAC__metadata_object_seektable_template_append_spaced_points");
	ex_FLAC__metadata_object_seektable_template_sort			= (FLAC__METADATA_OBJECT_SEEKTABLE_TEMPLATE_SORT) flacdll->GetFunctionAddress("FLAC__metadata_object_seektable_template_sort");

	ex_FLAC__VERSION_STRING							= (FLAC__VERSION_STRING_TYPE) flacdll->GetFunctionAddress("FLAC__VERSION_STRING");
	ex_FLAC__VENDOR_STRING							= (FLAC__VENDOR_STRING_TYPE) flacdll->GetFunctionAddress("FLAC__VENDOR_STRING");

	if (ex_FLAC_API_SUPPORTS_OGG_FLAC					== NIL ||

	    ex_FLAC__stream_encoder_new						== NIL ||
	    ex_FLAC__stream_encoder_delete					== NIL ||
	    ex_FLAC__stream_encoder_init_stream					== NIL ||
	    ex_FLAC__stream_encoder_init_ogg_stream				== NIL ||
	    ex_FLAC__stream_encoder_finish					== NIL ||
	    ex_FLAC__stream_encoder_set_channels				== NIL ||
	    ex_FLAC__stream_encoder_set_bits_per_sample				== NIL ||
	    ex_FLAC__stream_encoder_set_sample_rate				== NIL ||
	    ex_FLAC__stream_encoder_set_streamable_subset			== NIL ||
	    ex_FLAC__stream_encoder_set_do_mid_side_stereo			== NIL ||
	    ex_FLAC__stream_encoder_set_loose_mid_side_stereo			== NIL ||
	    ex_FLAC__stream_encoder_set_blocksize				== NIL ||
	    ex_FLAC__stream_encoder_set_max_lpc_order				== NIL ||
	    ex_FLAC__stream_encoder_set_qlp_coeff_precision			== NIL ||
	    ex_FLAC__stream_encoder_set_do_qlp_coeff_prec_search		== NIL ||
	    ex_FLAC__stream_encoder_set_do_exhaustive_model_search		== NIL ||
	    ex_FLAC__stream_encoder_set_min_residual_partition_order		== NIL ||
	    ex_FLAC__stream_encoder_set_max_residual_partition_order		== NIL ||
	    ex_FLAC__stream_encoder_set_metadata				== NIL ||
	    ex_FLAC__stream_encoder_set_compression_level			== NIL ||
	    ex_FLAC__stream_encoder_set_apodization				== NIL ||
	    ex_FLAC__stream_encoder_set_ogg_serial_number			== NIL ||
	    ex_FLAC__stream_encoder_process_interleaved				== NIL ||

	    ex_FLAC__metadata_object_new					== NIL ||
	    ex_FLAC__metadata_object_delete					== NIL ||

	    ex_FLAC__metadata_object_picture_set_mime_type			== NIL ||
	    ex_FLAC__metadata_object_picture_set_description			== NIL ||
	    ex_FLAC__metadata_object_picture_set_data				== NIL ||

	    ex_FLAC__metadata_object_cuesheet_track_new				== NIL ||
	    ex_FLAC__metadata_object_cuesheet_track_delete			== NIL ||
	    ex_FLAC__metadata_object_cuesheet_track_resize_indices		== NIL ||
	    ex_FLAC__metadata_object_cuesheet_track_insert_index		== NIL ||
	    ex_FLAC__metadata_object_cuesheet_resize_tracks			== NIL ||
	    ex_FLAC__metadata_object_cuesheet_insert_track			== NIL ||

	    ex_FLAC__metadata_object_vorbiscomment_append_comment		== NIL ||

	    ex_FLAC__metadata_object_seektable_template_append_spaced_points	== NIL ||
	    ex_FLAC__metadata_object_seektable_template_sort			== NIL ||

	    ex_FLAC__VERSION_STRING						== NIL ||
	    ex_FLAC__VENDOR_STRING						== NIL) { FreeFLACDLL(); return False; }

	return True;
}

Void FreeFLACDLL()
{
	BoCA::Utilities::FreeCodecDLL(flacdll);

	flacdll = NIL;
}
