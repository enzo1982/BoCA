########## BoCA directory makefile ##########

BOCA_PATH = ..

include $(BOCA_PATH)/Makefile-options

all:

clean:

install: uninstall
ifneq ($(BUILD_WIN32),True)
	mkdir -p $(PREFIX)/lib/boca

	cp -r boca $(PREFIX)/include
	cp boca.h $(PREFIX)/include

	chmod -R a=rX,u=rwX $(PREFIX)/include/boca
	chmod a=r,u=rw $(PREFIX)/include/boca.h
endif

uninstall:
ifneq ($(BUILD_WIN32),True)
	rm -f $(PREFIX)/include/boca.h
	rm -f -r $(PREFIX)/include/boca
	rm -f -r $(PREFIX)/lib/boca
endif
