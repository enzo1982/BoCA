########## BoCA directory makefile ##########

all:
ifeq ($(BUILD_UNIX),True)
	cd alsa && make
endif
ifeq ($(BUILD_WIN32),True)
	cd waveout && make
	cd winamp && make
endif

clean:
ifeq ($(BUILD_UNIX),True)
	cd alsa && make clean
endif
ifeq ($(BUILD_WIN32),True)
	cd waveout && make clean
	cd winamp && make clean
endif
