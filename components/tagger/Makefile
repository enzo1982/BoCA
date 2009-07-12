########## BoCA directory makefile ##########

all:
	cd apev2 && make
	cd flac && make
	cd id3v1 && make
	cd id3v2 && make
	cd mp4 && make
	cd vorbis && make
ifeq ($(BUILD_WIN32),True)
	cd wma && make
endif

clean:
	cd apev2 && make clean
	cd flac && make clean
	cd id3v1 && make clean
	cd id3v2 && make clean
	cd mp4 && make clean
	cd vorbis && make clean
ifeq ($(BUILD_WIN32),True)
	cd wma && make clean
endif
