########## BoCA directory makefile ##########

BOCA_PATH = ..

include $(BOCA_PATH)/Makefile-options

all:

clean:

install: uninstall
ifneq ($(BUILD_WIN32),True)
	cp -r boca $(PREFIX)/$(INCLUDE)
	cp boca.h $(PREFIX)/$(INCLUDE)

	chmod -R a=rX,u=rwX $(PREFIX)/$(INCLUDE)/boca
	chmod a=r,u=rw $(PREFIX)/$(INCLUDE)/boca.h
endif

uninstall:
ifneq ($(BUILD_WIN32),True)
	rm -f $(PREFIX)/$(INCLUDE)/boca.h
	rm -f -r $(PREFIX)/$(INCLUDE)/boca
endif
