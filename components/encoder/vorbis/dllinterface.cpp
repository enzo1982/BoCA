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

OGGSTREAMINIT			 ex_ogg_stream_init			= NIL;
OGGSTREAMPACKETIN		 ex_ogg_stream_packetin			= NIL;
OGGSTREAMFLUSH			 ex_ogg_stream_flush			= NIL;
OGGSTREAMPAGEOUT		 ex_ogg_stream_pageout			= NIL;
OGGPAGEEOS			 ex_ogg_page_eos			= NIL;
OGGSTREAMCLEAR			 ex_ogg_stream_clear			= NIL;

VORBISINFOINIT			 ex_vorbis_info_init			= NIL;
VORBISCOMMENTINIT		 ex_vorbis_comment_init			= NIL;
VORBISCOMMENTADDTAG		 ex_vorbis_comment_add_tag		= NIL;
VORBISANALYSISINIT		 ex_vorbis_analysis_init		= NIL;
VORBISBLOCKINIT			 ex_vorbis_block_init			= NIL;
VORBISANALYSISHEADEROUT		 ex_vorbis_analysis_headerout		= NIL;
VORBISANALYSISBUFFER		 ex_vorbis_analysis_buffer		= NIL;
VORBISANALYSISWROTE		 ex_vorbis_analysis_wrote		= NIL;
VORBISANALYSISBLOCKOUT		 ex_vorbis_analysis_blockout		= NIL;
VORBISANALYSIS			 ex_vorbis_analysis			= NIL;
VORBISBITRATEADDBLOCK		 ex_vorbis_bitrate_addblock		= NIL;
VORBISBITRATEFLUSHPACKET	 ex_vorbis_bitrate_flushpacket		= NIL;
VORBISBLOCKCLEAR		 ex_vorbis_block_clear			= NIL;
VORBISDSPCLEAR			 ex_vorbis_dsp_clear			= NIL;
VORBISCOMMENTCLEAR		 ex_vorbis_comment_clear		= NIL;
VORBISINFOCLEAR			 ex_vorbis_info_clear			= NIL;

VORBISENCODEINIT		 ex_vorbis_encode_init			= NIL;
VORBISENCODEINITVBR		 ex_vorbis_encode_init_vbr		= NIL;

DynamicLoader *oggdll		= NIL;
DynamicLoader *vorbisdll	= NIL;
DynamicLoader *vorbisencdll	= NIL;

Bool LoadOggDLL()
{
	oggdll = BoCA::Utilities::LoadCodecDLL("ogg");

	if (oggdll == NIL) return False;

	ex_ogg_stream_init		= (OGGSTREAMINIT) oggdll->GetFunctionAddress("ogg_stream_init");
	ex_ogg_stream_packetin		= (OGGSTREAMPACKETIN) oggdll->GetFunctionAddress("ogg_stream_packetin");
	ex_ogg_stream_flush		= (OGGSTREAMFLUSH) oggdll->GetFunctionAddress("ogg_stream_flush");
	ex_ogg_stream_pageout		= (OGGSTREAMPAGEOUT) oggdll->GetFunctionAddress("ogg_stream_pageout");
	ex_ogg_page_eos			= (OGGPAGEEOS) oggdll->GetFunctionAddress("ogg_page_eos");
	ex_ogg_stream_clear		= (OGGSTREAMCLEAR) oggdll->GetFunctionAddress("ogg_stream_clear");

	if (ex_ogg_stream_init			== NIL ||
	    ex_ogg_stream_packetin		== NIL ||
	    ex_ogg_stream_flush			== NIL ||
	    ex_ogg_stream_pageout		== NIL ||
	    ex_ogg_page_eos			== NIL ||
	    ex_ogg_stream_clear			== NIL) { FreeOggDLL(); return False; }

	return True;
}

Void FreeOggDLL()
{
	BoCA::Utilities::FreeCodecDLL(oggdll);

	oggdll = NIL;
}

Bool LoadVorbisDLL()
{
	vorbisdll = BoCA::Utilities::LoadCodecDLL("vorbis");

#ifdef __WIN32__
	vorbisencdll = vorbisdll;
#else
	vorbisencdll = BoCA::Utilities::LoadCodecDLL("vorbisenc");
#endif

	if (vorbisdll == NIL || vorbisencdll == NIL) return False;

	ex_vorbis_info_init		= (VORBISINFOINIT) vorbisdll->GetFunctionAddress("vorbis_info_init");
	ex_vorbis_comment_init		= (VORBISCOMMENTINIT) vorbisdll->GetFunctionAddress("vorbis_comment_init");
	ex_vorbis_comment_add_tag	= (VORBISCOMMENTADDTAG) vorbisdll->GetFunctionAddress("vorbis_comment_add_tag");
	ex_vorbis_analysis_init		= (VORBISANALYSISINIT) vorbisdll->GetFunctionAddress("vorbis_analysis_init");
	ex_vorbis_block_init		= (VORBISBLOCKINIT) vorbisdll->GetFunctionAddress("vorbis_block_init");
	ex_vorbis_analysis_headerout	= (VORBISANALYSISHEADEROUT) vorbisdll->GetFunctionAddress("vorbis_analysis_headerout");
	ex_vorbis_analysis_buffer	= (VORBISANALYSISBUFFER) vorbisdll->GetFunctionAddress("vorbis_analysis_buffer");
	ex_vorbis_analysis_wrote	= (VORBISANALYSISWROTE) vorbisdll->GetFunctionAddress("vorbis_analysis_wrote");
	ex_vorbis_analysis_blockout	= (VORBISANALYSISBLOCKOUT) vorbisdll->GetFunctionAddress("vorbis_analysis_blockout");
	ex_vorbis_analysis		= (VORBISANALYSIS) vorbisdll->GetFunctionAddress("vorbis_analysis");
	ex_vorbis_bitrate_addblock	= (VORBISBITRATEADDBLOCK) vorbisdll->GetFunctionAddress("vorbis_bitrate_addblock");
	ex_vorbis_bitrate_flushpacket	= (VORBISBITRATEFLUSHPACKET) vorbisdll->GetFunctionAddress("vorbis_bitrate_flushpacket");
	ex_vorbis_block_clear		= (VORBISBLOCKCLEAR) vorbisdll->GetFunctionAddress("vorbis_block_clear");
	ex_vorbis_dsp_clear		= (VORBISDSPCLEAR) vorbisdll->GetFunctionAddress("vorbis_dsp_clear");
	ex_vorbis_comment_clear		= (VORBISCOMMENTCLEAR) vorbisdll->GetFunctionAddress("vorbis_comment_clear");
	ex_vorbis_info_clear		= (VORBISINFOCLEAR) vorbisdll->GetFunctionAddress("vorbis_info_clear");

	ex_vorbis_encode_init		= (VORBISENCODEINIT) vorbisencdll->GetFunctionAddress("vorbis_encode_init");
	ex_vorbis_encode_init_vbr	= (VORBISENCODEINITVBR) vorbisencdll->GetFunctionAddress("vorbis_encode_init_vbr");

	if (ex_vorbis_info_init			== NIL ||
	    ex_vorbis_comment_init		== NIL ||
	    ex_vorbis_comment_add_tag		== NIL ||
	    ex_vorbis_analysis_init		== NIL ||
	    ex_vorbis_block_init		== NIL ||
	    ex_vorbis_analysis_headerout	== NIL ||
	    ex_vorbis_analysis_buffer		== NIL ||
	    ex_vorbis_analysis_wrote		== NIL ||
	    ex_vorbis_analysis_blockout		== NIL ||
	    ex_vorbis_analysis			== NIL ||
	    ex_vorbis_bitrate_addblock		== NIL ||
	    ex_vorbis_bitrate_flushpacket	== NIL ||
	    ex_vorbis_block_clear		== NIL ||
	    ex_vorbis_dsp_clear			== NIL ||
	    ex_vorbis_comment_clear		== NIL ||
	    ex_vorbis_info_clear		== NIL ||
	    ex_vorbis_encode_init		== NIL ||
	    ex_vorbis_encode_init_vbr		== NIL) { FreeVorbisDLL(); return False; }

	return True;
}

Void FreeVorbisDLL()
{
	BoCA::Utilities::FreeCodecDLL(vorbisdll);

#ifndef __WIN32__
	BoCA::Utilities::FreeCodecDLL(vorbisencdll);
#endif

	vorbisdll    = NIL;
	vorbisencdll = NIL;
}
