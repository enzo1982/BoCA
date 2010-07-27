########## BoCA directory makefile ##########

all:
ifeq ($(BUILD_WIN32),True)
	cd bladeenc && make
endif
	cd bonk && make
	cd faac && make
	cd flac && make
	cd lame && make
ifeq ($(BUILD_WIN32),True)
	cd mac && make
endif
	cd speex && make
ifeq ($(BUILD_WIN32),True)
	cd twinvq && make
endif
	cd vorbis && make
	cd wave && make
ifeq ($(BUILD_WIN32),True)
	cd wma && make
endif

clean:
ifeq ($(BUILD_WIN32),True)
	cd bladeenc && make clean
endif
	cd bonk && make clean
	cd faac && make clean
	cd flac && make clean
	cd lame && make clean
ifeq ($(BUILD_WIN32),True)
	cd mac && make clean
endif
	cd speex && make clean
ifeq ($(BUILD_WIN32),True)
	cd twinvq && make clean
endif
	cd vorbis && make clean
	cd wave && make clean
ifeq ($(BUILD_WIN32),True)
	cd wma && make clean
endif
