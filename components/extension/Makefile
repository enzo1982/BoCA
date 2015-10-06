########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(BOCA_PATH)/Makefile-options

all:
	cd donate && $(MAKE)
	cd langnotify && $(MAKE)
	cd protocols && $(MAKE)
	cd statustime && $(MAKE)
	cd tagedit && $(MAKE)
	cd youtube && $(MAKE)

clean:
	cd donate && $(MAKE) clean
	cd langnotify && $(MAKE) clean
	cd protocols && $(MAKE) clean
	cd statustime && $(MAKE) clean
	cd tagedit && $(MAKE) clean
	cd youtube && $(MAKE) clean

install:
	cd donate && $(MAKE) install
	cd langnotify && $(MAKE) install
	cd statustime && $(MAKE) install
	cd tagedit && $(MAKE) install
	cd youtube && $(MAKE) install

uninstall:
	cd donate && $(MAKE) uninstall
	cd langnotify && $(MAKE) uninstall
	cd statustime && $(MAKE) uninstall
	cd tagedit && $(MAKE) uninstall
	cd youtube && $(MAKE) uninstall
