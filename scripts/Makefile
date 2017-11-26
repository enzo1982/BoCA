########## BoCA CDK makefile ##########

BOCA_PATH = ..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

install:
	$(INSTALL) -d $(DESTDIR)$(libdir)/boca

	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_alac.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_als.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_ac3.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_alac.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_amr.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_atrac1.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_atrac3.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_avi_audio.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_dss.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_dts.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_flv_audio.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_m4v_audio.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_mac.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_mkv_audio.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_mlp.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_mov_audio.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_mpc.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_mpg_audio.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_purevoice.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_realaudio.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_tak.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_wavpack.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_webm_audio.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_wma.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_avconv_wmv_audio.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_la.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_mpc.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_optimfrog.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_rkau.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_shorten.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_timidity.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_decoder_tta.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_aften.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_als.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_avconv_alac.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_avconv_wma.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_ffdcaenc.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_flake.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_la.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_mpc.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_neroaac.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_optimfrog.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_rkau.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_shorten.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_tta.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/boca_encoder_twolame.1.0.xml $(DESTDIR)$(libdir)/boca

	$(INSTALL_DATA) $(SRCDIR)/unix/boca_decoder_wavpack.1.0.xml $(DESTDIR)$(libdir)/boca
	$(INSTALL_DATA) $(SRCDIR)/unix/boca_encoder_wavpack.1.0.xml $(DESTDIR)$(libdir)/boca

uninstall:
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_alac.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_als.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_ac3.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_alac.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_amr.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_atrac1.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_atrac3.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_avi_audio.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_dss.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_dts.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_flv_audio.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_m4v_audio.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_mac.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_mkv_audio.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_mlp.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_mov_audio.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_mpc.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_mpg_audio.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_purevoice.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_realaudio.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_tak.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_wavpack.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_webm_audio.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_wma.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_avconv_wmv_audio.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_la.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_mpc.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_optimfrog.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_rkau.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_shorten.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_timidity.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_tta.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_decoder_wavpack.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_aften.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_als.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_avconv_alac.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_avconv_wma.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_ffdcaenc.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_flake.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_la.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_mpc.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_neroaac.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_optimfrog.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_rkau.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_shorten.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_tta.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_twolame.1.0.xml
	rm -f $(DESTDIR)$(libdir)/boca/boca_encoder_wavpack.1.0.xml
