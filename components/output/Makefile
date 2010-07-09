########## BoCA directory makefile ##########

all:
ifeq ($(BUILD_WIN32),True)
	cd waveout && make
	cd winamp && make
endif

clean:
ifeq ($(BUILD_WIN32),True)
	cd waveout && make clean
	cd winamp && make clean
endif
