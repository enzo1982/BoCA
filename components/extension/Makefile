########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(BOCA_PATH)/Makefile-options

all:
	cd donate && $(MAKE)
	cd protocols && $(MAKE)
	cd statustime && $(MAKE)
	cd tagedit && $(MAKE)
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_FREEBSD),True)
	cd youtube && $(MAKE)
endif
endif

clean:
	cd donate && $(MAKE) clean
	cd protocols && $(MAKE) clean
	cd statustime && $(MAKE) clean
	cd tagedit && $(MAKE) clean
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_FREEBSD),True)
	cd youtube && $(MAKE) clean
endif
endif
