########## BoCA directory makefile ##########

BOCA_PATH = ..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

all:

clean:

install: all
ifneq ($(BUILD_WIN32),True)
	$(INSTALL) -d "$(DESTDIR)"$(includedir)

	cp -r "$(SRCDIR)"/boca "$(DESTDIR)"$(includedir)
	chmod -R a=rX,u=rwX "$(DESTDIR)"$(includedir)/boca

	$(INSTALL_DATA) "$(SRCDIR)"/boca.h "$(DESTDIR)"$(includedir)
endif

uninstall:
ifneq ($(BUILD_WIN32),True)
	rm -f "$(DESTDIR)"$(includedir)/boca.h
	rm -f -r "$(DESTDIR)"$(includedir)/boca
endif
