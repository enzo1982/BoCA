 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dllinterface.h"

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
VORBISSYNTHESISINIT		 ex_vorbis_synthesis_init		= NIL;
VORBISSYNTHESIS			 ex_vorbis_synthesis			= NIL;
VORBISSYNTHESISBLOCKIN		 ex_vorbis_synthesis_blockin		= NIL;
VORBISSYNTHESISPCMOUT		 ex_vorbis_synthesis_pcmout		= NIL;
VORBISSYNTHESISREAD		 ex_vorbis_synthesis_read		= NIL;
VORBISSYNTHESISHEADERIN		 ex_vorbis_synthesis_headerin		= NIL;
VORBISBLOCKCLEAR		 ex_vorbis_block_clear			= NIL;
VORBISDSPCLEAR			 ex_vorbis_dsp_clear			= NIL;
VORBISCOMMENTCLEAR		 ex_vorbis_comment_clear		= NIL;
VORBISINFOCLEAR			 ex_vorbis_info_clear			= NIL;

OGGSTREAMINIT			 ex_ogg_stream_init			= NIL;
OGGSTREAMPACKETIN		 ex_ogg_stream_packetin			= NIL;
OGGSTREAMPACKETOUT		 ex_ogg_stream_packetout		= NIL;
OGGSTREAMFLUSH			 ex_ogg_stream_flush			= NIL;
OGGSTREAMPAGEIN			 ex_ogg_stream_pagein			= NIL;
OGGSTREAMPAGEOUT		 ex_ogg_stream_pageout			= NIL;
OGGPAGEEOS			 ex_ogg_page_eos			= NIL;
OGGPAGESERIALNO			 ex_ogg_page_serialno			= NIL;
OGGSTREAMCLEAR			 ex_ogg_stream_clear			= NIL;
OGGSYNCINIT			 ex_ogg_sync_init			= NIL;
OGGSYNCBUFFER			 ex_ogg_sync_buffer			= NIL;
OGGSYNCWROTE			 ex_ogg_sync_wrote			= NIL;
OGGSYNCPAGEOUT			 ex_ogg_sync_pageout			= NIL;
OGGSYNCCLEAR			 ex_ogg_sync_clear			= NIL;

DynamicLoader *vorbisdll	= NIL;

Bool LoadVorbisDLL()
{
	vorbisdll = new DynamicLoader("codecs/OggVorbis");

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
	ex_vorbis_synthesis_init	= (VORBISSYNTHESISINIT) vorbisdll->GetFunctionAddress("vorbis_synthesis_init");
	ex_vorbis_synthesis		= (VORBISSYNTHESIS) vorbisdll->GetFunctionAddress("vorbis_synthesis");
	ex_vorbis_synthesis_blockin	= (VORBISSYNTHESISBLOCKIN) vorbisdll->GetFunctionAddress("vorbis_synthesis_blockin");
	ex_vorbis_synthesis_pcmout	= (VORBISSYNTHESISPCMOUT) vorbisdll->GetFunctionAddress("vorbis_synthesis_pcmout");
	ex_vorbis_synthesis_read	= (VORBISSYNTHESISREAD) vorbisdll->GetFunctionAddress("vorbis_synthesis_read");
	ex_vorbis_synthesis_headerin	= (VORBISSYNTHESISHEADERIN) vorbisdll->GetFunctionAddress("vorbis_synthesis_headerin");
	ex_vorbis_block_clear		= (VORBISBLOCKCLEAR) vorbisdll->GetFunctionAddress("vorbis_block_clear");
	ex_vorbis_dsp_clear		= (VORBISDSPCLEAR) vorbisdll->GetFunctionAddress("vorbis_dsp_clear");
	ex_vorbis_comment_clear		= (VORBISCOMMENTCLEAR) vorbisdll->GetFunctionAddress("vorbis_comment_clear");
	ex_vorbis_info_clear		= (VORBISINFOCLEAR) vorbisdll->GetFunctionAddress("vorbis_info_clear");
	ex_ogg_stream_init		= (OGGSTREAMINIT) vorbisdll->GetFunctionAddress("ogg_stream_init");
	ex_ogg_stream_packetin		= (OGGSTREAMPACKETIN) vorbisdll->GetFunctionAddress("ogg_stream_packetin");
	ex_ogg_stream_packetout		= (OGGSTREAMPACKETOUT) vorbisdll->GetFunctionAddress("ogg_stream_packetout");
	ex_ogg_stream_flush		= (OGGSTREAMFLUSH) vorbisdll->GetFunctionAddress("ogg_stream_flush");
	ex_ogg_stream_pagein		= (OGGSTREAMPAGEIN) vorbisdll->GetFunctionAddress("ogg_stream_pagein");
	ex_ogg_stream_pageout		= (OGGSTREAMPAGEOUT) vorbisdll->GetFunctionAddress("ogg_stream_pageout");
	ex_ogg_page_eos			= (OGGPAGEEOS) vorbisdll->GetFunctionAddress("ogg_page_eos");
	ex_ogg_page_serialno		= (OGGPAGESERIALNO) vorbisdll->GetFunctionAddress("ogg_page_serialno");
	ex_ogg_stream_clear		= (OGGSTREAMCLEAR) vorbisdll->GetFunctionAddress("ogg_stream_clear");
	ex_ogg_sync_init		= (OGGSYNCINIT) vorbisdll->GetFunctionAddress("ogg_sync_init");
	ex_ogg_sync_buffer		= (OGGSYNCBUFFER) vorbisdll->GetFunctionAddress("ogg_sync_buffer");
	ex_ogg_sync_wrote		= (OGGSYNCWROTE) vorbisdll->GetFunctionAddress("ogg_sync_wrote");
	ex_ogg_sync_pageout		= (OGGSYNCPAGEOUT) vorbisdll->GetFunctionAddress("ogg_sync_pageout");
	ex_ogg_sync_clear		= (OGGSYNCCLEAR) vorbisdll->GetFunctionAddress("ogg_sync_clear");

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
	    ex_vorbis_synthesis_init		== NIL ||
	    ex_vorbis_synthesis			== NIL ||
	    ex_vorbis_synthesis_blockin		== NIL ||
	    ex_vorbis_synthesis_pcmout		== NIL ||
	    ex_vorbis_synthesis_read		== NIL ||
	    ex_vorbis_synthesis_headerin	== NIL ||
	    ex_vorbis_block_clear		== NIL ||
	    ex_vorbis_dsp_clear			== NIL ||
	    ex_vorbis_comment_clear		== NIL ||
	    ex_vorbis_info_clear		== NIL ||
	    ex_ogg_stream_init			== NIL ||
	    ex_ogg_stream_packetin		== NIL ||
	    ex_ogg_stream_packetout		== NIL ||
	    ex_ogg_stream_flush			== NIL ||
	    ex_ogg_stream_pagein		== NIL ||
	    ex_ogg_stream_pageout		== NIL ||
	    ex_ogg_page_eos			== NIL ||
	    ex_ogg_page_serialno		== NIL ||
	    ex_ogg_stream_clear			== NIL ||
	    ex_ogg_sync_init			== NIL ||
	    ex_ogg_sync_buffer			== NIL ||
	    ex_ogg_sync_wrote			== NIL ||
	    ex_ogg_sync_pageout			== NIL ||
	    ex_ogg_sync_clear			== NIL) { FreeVorbisDLL(); return False; }

	return True;
}

Void FreeVorbisDLL()
{
	Object::DeleteObject(vorbisdll);

	vorbisdll = NIL;
}
