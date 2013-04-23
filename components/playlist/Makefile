########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(BOCA_PATH)/Makefile-options

all:
	cd cuesheet && $(MAKE)
	cd m3u && $(MAKE)
	cd pls && $(MAKE)
	cd xspf && $(MAKE)

clean:
	cd cuesheet && $(MAKE) clean
	cd m3u && $(MAKE) clean
	cd pls && $(MAKE) clean
	cd xspf && $(MAKE) clean

install:
	cd cuesheet && $(MAKE) install
	cd m3u && $(MAKE) install
	cd pls && $(MAKE) install
	cd xspf && $(MAKE) install

uninstall:
	cd cuesheet && $(MAKE) uninstall
	cd m3u && $(MAKE) uninstall
	cd pls && $(MAKE) uninstall
	cd xspf && $(MAKE) uninstall
