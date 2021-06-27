########## BoCA CDK makefile ##########

# Change these variables to fit your project:
TARGET	  = boca
TYPE	  = runtime
VERSION	  = 1.0
REVISION  = 3

BOCA_PATH = ..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = application/external/configlayer.o application/external/decodercomponent.o application/external/encodercomponent.o
OBJECTS	 += application/component.o application/componentspecs.o application/decodercomponent.o application/deviceinfocomponent.o application/dspcomponent.o application/encodercomponent.o application/extensioncomponent.o application/outputcomponent.o application/playlistcomponent.o application/registry.o application/streamcomponent.o application/taggercomponent.o application/verifiercomponent.o
OBJECTS	 += common/communication/application.o common/communication/engine.o common/communication/joblist.o common/communication/menu.o common/communication/settings.o
OBJECTS	 += common/metadata/device.o common/metadata/format.o common/metadata/info.o common/metadata/mcdi.o common/metadata/picture.o common/metadata/picturedata.o common/metadata/track.o
OBJECTS	 += common/config.o common/configlayer.o common/formatconverter.o common/i18n.o common/protocol.o common/utilities.o
OBJECTS	 += component/component.o component/decodercomponent.o component/deviceinfocomponent.o component/dspcomponent.o component/encodercomponent.o component/extensioncomponent.o component/outputcomponent.o component/playlistcomponent.o component/streamcomponent.o component/taggercomponent.o component/verifiercomponent.o
OBJECTS	 += core/core.o core/main.o

ifeq ($(BUILD_WIN32),True)
	OBJECTS += application/external/win32/decodercomponentfile.o application/external/win32/decodercomponentstdio.o application/external/win32/encodercomponentfile.o application/external/win32/encodercomponentstdio.o
else ifeq ($(BUILD_UNIX),True)
	OBJECTS += application/external/posix/decodercomponentfile.o application/external/posix/decodercomponentstdio.o application/external/posix/encodercomponentfile.o application/external/posix/encodercomponentstdio.o
endif

# Enter additional library dependencies here
LIBS	  =

# Enter addition commands for targets all and clean here:
ALLCMD1   =
ALLCMD2   =
CLEANCMD1 =
CLEANCMD1 =

## Do not change anything below this line. ##

CCOPTS	  = -fvisibility=hidden -DBOCA_CDK_BUILD -DBOCA_INSTALL_PREFIX=\"$(prefix)\" -I"$(SRCDIR)"/$(BOCA_PATH)/include
LDOPTS	  = -L"$(SRCDIR)"/$(BOCA_PATH)/$(LIBDIR) -lstdc++ $(LIBS)

ifeq ($(BUILD_WIN32),True)
	LDOPTS += -lsmooth -lws2_32 -Wl,--dynamicbase,--nxcompat,--enable-auto-import
else
	CCOPTS += -fPIC

	ifneq ($(BUILD_SOLARIS),True)
	ifneq ($(BUILD_HAIKU),True)
		CCOPTS += -pthread

		ifneq ($(BUILD_OSX),True)
			ifeq ($(BUILD_NETBSD),True)
				CCOPTS += -I/usr/pkg/include
			else
				CCOPTS += -I/usr/local/include
			endif
		endif
	endif
	endif

	LDOPTS += -L$(prefix)/lib -lsmooth-$(SMOOTHVER)

	ifeq ($(BUILD_OSX),True)
		LDOPTS += -Wl,-dylib_install_name,libboca-$(VERSION).$(REVISION)$(SHARED)
	else
		LDOPTS += -Wl,-soname,libboca-$(VERSION)$(SHARED).$(REVISION)
	endif
endif

all: folders $(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(SHARED)

folders:
ifeq ($(BUILD_WIN32),True)
	mkdir -p application/external/win32
else
	mkdir -p application/external/posix
endif
	mkdir -p common/communication common/metadata component core

$(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(SHARED): $(OBJECTS)
ifeq ($(BUILD_WIN32),True)
	$(LD) --shared -o $@ $(OBJECTS) -Wl,--out-implib,$(BOCA_PATH)/$(LIBDIR)/libboca.a $(LDOPTS) $(LDFLAGS)
else
ifneq ($(BUILD_OSX),True)
	$(LD) --shared -o $@ $(OBJECTS) $(LDOPTS) $(LDFLAGS)
else
	$(LD) -dynamiclib -o $@ $(OBJECTS) $(LDOPTS) $(LDFLAGS)
endif

	rm -f $(BOCA_PATH)/$(LIBDIR)/libboca-$(VERSION)$(SHARED)
	ln -fs ../$(BINDIR)/boca.$(VERSION)$(SHARED) $(BOCA_PATH)/$(LIBDIR)/libboca-$(VERSION)$(SHARED)
endif

clean:
	rm -f $(OBJECTS)
	rm -f $(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(SHARED)

ifneq ("$(SRCDIR)","$(CURDIR)")
ifeq ($(BUILD_WIN32),True)
	rmdir application/external/win32 || true
else
	rmdir application/external/posix || true
endif

	rmdir application/external application common/communication common/metadata common component core || true
endif

ifeq ($(BUILD_WIN32),True)
	rm -f $(BOCA_PATH)/$(LIBDIR)/libboca.a
else
	rm -f $(BOCA_PATH)/$(LIBDIR)/libboca-$(VERSION)$(SHARED)
endif

install: all
ifneq ($(BUILD_WIN32),True)
	$(INSTALL) -d "$(DESTDIR)"$(libdir)/boca
	$(INSTALL_DATA) $(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(SHARED) "$(DESTDIR)"$(libdir)/boca
	ln -fs boca/boca.$(VERSION)$(SHARED) "$(DESTDIR)"$(libdir)/libboca-$(VERSION)$(SHARED)

ifeq ($(BUILD_OSX),True)
	ln -fs boca/boca.$(VERSION)$(SHARED) "$(DESTDIR)"$(libdir)/libboca-$(VERSION).$(REVISION)$(SHARED)
else
	ln -fs boca/boca.$(VERSION)$(SHARED) "$(DESTDIR)"$(libdir)/libboca-$(VERSION)$(SHARED).$(REVISION)
endif

ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_HAIKU),True)
ifeq ("$(DESTDIR)","")
	$(LDCONFIG) 2> /dev/null || true
endif
endif
endif
endif
endif

uninstall:
ifneq ($(BUILD_WIN32),True)
ifeq ($(BUILD_OSX),True)
	rm -f "$(DESTDIR)"$(libdir)/libboca-$(VERSION).$(REVISION)$(SHARED)
else
	rm -f "$(DESTDIR)"$(libdir)/libboca-$(VERSION)$(SHARED).$(REVISION)
endif

	rm -f "$(DESTDIR)"$(libdir)/libboca-$(VERSION)$(SHARED)
	rm -f "$(DESTDIR)"$(libdir)/boca/boca.$(VERSION)$(SHARED)

ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_HAIKU),True)
ifeq ("$(DESTDIR)","")
	$(LDCONFIG) 2> /dev/null || true
endif
endif
endif
endif
endif

.c.o:
	$(CC) $(CCOPTS) $(CFLAGS) -c $< -o $@

.cpp.o:
	$(CXX) $(CCOPTS) $(CXXFLAGS) -c $< -o $@

.m.o:
	$(OBJC) $(CCOPTS) $(OBJCFLAGS) -c $< -o $@

.mm.o:
	$(OBJCXX) $(CCOPTS) $(OBJCXXFLAGS) -c $< -o $@
