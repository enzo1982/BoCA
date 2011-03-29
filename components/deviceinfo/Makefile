########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(BOCA_PATH)/Makefile-options

all:
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_SOLARIS),True)
	cd cdio && $(MAKE)
ifneq ($(BUILD_OSX),True)
	cd cdparanoia && $(MAKE)
endif
endif
endif
ifeq ($(BUILD_WIN32),True)
	cd cdrip && $(MAKE)
endif

clean:
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_SOLARIS),True)
	cd cdio && $(MAKE) clean
ifneq ($(BUILD_OSX),True)
	cd cdparanoia && $(MAKE) clean
endif
endif
endif
ifeq ($(BUILD_WIN32),True)
	cd cdrip && $(MAKE) clean
endif
