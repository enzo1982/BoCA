########## BoCA directory makefile ##########

all:
ifeq ($(BUILD_UNIX),True)
	cd cdparanoia && make
endif
ifeq ($(BUILD_WIN32),True)
	cd cdrip && make
endif

clean:
ifeq ($(BUILD_UNIX),True)
	cd cdparanoia && make clean
endif
ifeq ($(BUILD_WIN32),True)
	cd cdrip && make clean
endif
