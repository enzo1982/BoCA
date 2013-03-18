########## BoCA directory makefile ##########

include Makefile-options

all:
	cd runtime && $(MAKE) -j 4
	cd components && $(MAKE) -j 4

clean:
	cd runtime && $(MAKE) clean
	cd components && $(MAKE) clean

install:
ifneq ($(BUILD_WIN32),True)
	cd include && $(MAKE) install
	cd runtime && $(MAKE) install
	cd scripts && $(MAKE) install
	cd components && $(MAKE) install
endif

uninstall:
ifneq ($(BUILD_WIN32),True)
	cd include && $(MAKE) uninstall
	cd runtime && $(MAKE) uninstall
	cd scripts && $(MAKE) uninstall
	cd components && $(MAKE) uninstall
endif
