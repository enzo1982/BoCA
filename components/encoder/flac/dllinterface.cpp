 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

using namespace BoCA;

FLAC__STREAM_ENCODER_NEW					 ex_FLAC__stream_encoder_new						= NIL;
FLAC__STREAM_ENCODER_DELETE					 ex_FLAC__stream_encoder_delete						= NIL;
FLAC__STREAM_ENCODER_INIT_STREAM				 ex_FLAC__stream_encoder_init_stream					= NIL;
FLAC__STREAM_ENCODER_FINISH					 ex_FLAC__stream_encoder_finish						= NIL;
FLAC__STREAM_ENCODER_SET_CHANNELS				 ex_FLAC__stream_encoder_set_channels					= NIL;
FLAC__STREAM_ENCODER_SET_BITS_PER_SAMPLE			 ex_FLAC__stream_encoder_set_bits_per_sample				= NIL;
FLAC__STREAM_ENCODER_SET_SAMPLE_RATE				 ex_FLAC__stream_encoder_set_sample_rate				= NIL;
FLAC__STREAM_ENCODER_SET_STREAMABLE_SUBSET			 ex_FLAC__stream_encoder_set_streamable_subset				= NIL;
FLAC__STREAM_ENCODER_SET_DO_MID_SIDE_STEREO			 ex_FLAC__stream_encoder_set_do_mid_side_stereo				= NIL;
FLAC__STREAM_ENCODER_SET_LOOSE_MID_SIDE_STEREO			 ex_FLAC__stream_encoder_set_loose_mid_side_stereo			= NIL;
FLAC__STREAM_ENCODER_SET_BLOCKSIZE				 ex_FLAC__stream_encoder_set_blocksize					= NIL;
FLAC__STREAM_ENCODER_SET_MAX_LPC_ORDER				 ex_FLAC__stream_encoder_set_max_lpc_order				= NIL;
FLAC__STREAM_ENCODER_SET_QLP_COEFF_PRECISION			 ex_FLAC__stream_encoder_set_qlp_coeff_precision			= NIL;
FLAC__STREAM_ENCODER_SET_DO_QLP_COEFF_PREC_SEARCH		 ex_FLAC__stream_encoder_set_do_qlp_coeff_prec_search			= NIL;
FLAC__STREAM_ENCODER_SET_DO_EXHAUSTIVE_MODEL_SEARCH		 ex_FLAC__stream_encoder_set_do_exhaustive_model_search			= NIL;
FLAC__STREAM_ENCODER_SET_MIN_RESIDUAL_PARTITION_ORDER		 ex_FLAC__stream_encoder_set_min_residual_partition_order		= NIL;
FLAC__STREAM_ENCODER_SET_MAX_RESIDUAL_PARTITION_ORDER		 ex_FLAC__stream_encoder_set_max_residual_partition_order		= NIL;
FLAC__STREAM_ENCODER_SET_METADATA				 ex_FLAC__stream_encoder_set_metadata					= NIL;
FLAC__STREAM_ENCODER_SET_COMPRESSION_LEVEL			 ex_FLAC__stream_encoder_set_compression_level				= NIL;
FLAC__STREAM_ENCODER_SET_APODIZATION				 ex_FLAC__stream_encoder_set_apodization				= NIL;
FLAC__STREAM_ENCODER_PROCESS_INTERLEAVED			 ex_FLAC__stream_encoder_process_interleaved				= NIL;
FLAC__METADATA_OBJECT_NEW					 ex_FLAC__metadata_object_new						= NIL;
FLAC__METADATA_OBJECT_DELETE					 ex_FLAC__metadata_object_delete					= NIL;
FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE			 ex_FLAC__metadata_object_picture_set_mime_type				= NIL;
FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION			 ex_FLAC__metadata_object_picture_set_description			= NIL;
FLAC__METADATA_OBJECT_PICTURE_SET_DATA				 ex_FLAC__metadata_object_picture_set_data				= NIL;
FLAC__METADATA_OBJECT_VORBISCOMMENT_RESIZE_COMMENTS		 ex_FLAC__metadata_object_vorbiscomment_resize_comments			= NIL;
FLAC__VENDOR_STRING_TYPE					 ex_FLAC__VENDOR_STRING							= NIL;
FLAC__VERSION_STRING_TYPE					 ex_FLAC__VERSION_STRING						= NIL;

DynamicLoader *flacdll	= NIL;

Bool LoadFLACDLL()
{
	if (Config::Get()->GetIntValue("OpenMP", "EnableOpenMP", True) && CPU().GetNumCores() >= 2 && CPU().HasSSE3())
	{
#ifdef __WIN32__
		if (!File(String(GUI::Application::GetApplicationDirectory()).Append("codecs\\FLAC-OpenMP.dll")).Exists()) return False;
#endif

		flacdll = new DynamicLoader("codecs/FLAC-OpenMP");

		if (flacdll->GetSystemModuleHandle() == NIL) FreeFLACDLL();
	}

	if (flacdll == NIL)
	{
#ifdef __WIN32__
		if (!File(String(GUI::Application::GetApplicationDirectory()).Append("codecs\\FLAC.dll")).Exists()) return False;
#endif

		flacdll = new DynamicLoader("codecs/FLAC");
	}

	ex_FLAC__stream_encoder_new						= (FLAC__STREAM_ENCODER_NEW) flacdll->GetFunctionAddress("FLAC__stream_encoder_new");
	ex_FLAC__stream_encoder_delete						= (FLAC__STREAM_ENCODER_DELETE) flacdll->GetFunctionAddress("FLAC__stream_encoder_delete");
	ex_FLAC__stream_encoder_init_stream					= (FLAC__STREAM_ENCODER_INIT_STREAM) flacdll->GetFunctionAddress("FLAC__stream_encoder_init_stream");
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
	ex_FLAC__stream_encoder_process_interleaved				= (FLAC__STREAM_ENCODER_PROCESS_INTERLEAVED) flacdll->GetFunctionAddress("FLAC__stream_encoder_process_interleaved");
	ex_FLAC__metadata_object_new						= (FLAC__METADATA_OBJECT_NEW) flacdll->GetFunctionAddress("FLAC__metadata_object_new");
	ex_FLAC__metadata_object_delete						= (FLAC__METADATA_OBJECT_DELETE) flacdll->GetFunctionAddress("FLAC__metadata_object_delete");
	ex_FLAC__metadata_object_picture_set_mime_type				= (FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE) flacdll->GetFunctionAddress("FLAC__metadata_object_picture_set_mime_type");
	ex_FLAC__metadata_object_picture_set_description			= (FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION) flacdll->GetFunctionAddress("FLAC__metadata_object_picture_set_description");
	ex_FLAC__metadata_object_picture_set_data				= (FLAC__METADATA_OBJECT_PICTURE_SET_DATA) flacdll->GetFunctionAddress("FLAC__metadata_object_picture_set_data");
	ex_FLAC__metadata_object_vorbiscomment_resize_comments			= (FLAC__METADATA_OBJECT_VORBISCOMMENT_RESIZE_COMMENTS) flacdll->GetFunctionAddress("FLAC__metadata_object_vorbiscomment_resize_comments");
	ex_FLAC__VENDOR_STRING							= (FLAC__VENDOR_STRING_TYPE) flacdll->GetFunctionAddress("FLAC__VENDOR_STRING");
	ex_FLAC__VERSION_STRING							= (FLAC__VERSION_STRING_TYPE) flacdll->GetFunctionAddress("FLAC__VERSION_STRING");

	if (ex_FLAC__stream_encoder_new						== NIL ||
	    ex_FLAC__stream_encoder_delete					== NIL ||
	    ex_FLAC__stream_encoder_init_stream					== NIL ||
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
	    ex_FLAC__stream_encoder_process_interleaved				== NIL ||
	    ex_FLAC__metadata_object_new					== NIL ||
	    ex_FLAC__metadata_object_delete					== NIL ||
	    ex_FLAC__metadata_object_picture_set_mime_type			== NIL ||
	    ex_FLAC__metadata_object_picture_set_description			== NIL ||
	    ex_FLAC__metadata_object_picture_set_data				== NIL ||
	    ex_FLAC__metadata_object_vorbiscomment_resize_comments		== NIL ||
	    ex_FLAC__VENDOR_STRING						== NIL ||
	    ex_FLAC__VERSION_STRING						== NIL) { FreeFLACDLL(); return False; }

	return True;
}

Void FreeFLACDLL()
{
	Object::DeleteObject(flacdll);

	flacdll = NIL;
}
