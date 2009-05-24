########## BoCA directory makefile ##########

all:
	cd aiff && make
	cd au && make
	cd bonk && make
ifeq ($(BUILD_WIN32),True)
	cd cdrip && make
endif
	cd faad2 && make
	cd flac && make
	cd lame && make
ifeq ($(BUILD_WIN32),True)
	cd mac && make
endif
	cd mad && make
	cd sndfile && make
	cd speex && make
ifeq ($(BUILD_WIN32),True)
	cd twinvq && make
endif
	cd voc && make
	cd vorbis && make
	cd wave && make
ifeq ($(BUILD_WIN32),True)
	cd winamp && make
endif

clean:
	cd aiff && make clean
	cd au && make clean
	cd bonk && make clean
ifeq ($(BUILD_WIN32),True)
	cd cdrip && make clean
endif
	cd faad2 && make clean
	cd flac && make clean
	cd lame && make clean
	cd mac && make clean
	cd mad && make clean
	cd sndfile && make clean
	cd speex && make clean
ifeq ($(BUILD_WIN32),True)
	cd twinvq && make clean
endif
	cd voc && make clean
	cd vorbis && make clean
	cd wave && make clean
ifeq ($(BUILD_WIN32),True)
	cd winamp && make clean
endif
