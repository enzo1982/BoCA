########## BoCA CDK makefile ##########

# Change these variables to fit your project:
TARGET	  = boca
TYPE	  = runtime
VERSION	  = 1.0
SMOOTHVER = 0.8.71

BOCA_PATH = ..

include $(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = application/external/configlayer.o application/external/decodercomponent.o application/external/encodercomponent.o application/component.o application/componentspecs.o application/decodercomponent.o application/deviceinfocomponent.o application/dspcomponent.o application/encodercomponent.o application/extensioncomponent.o application/outputcomponent.o application/playlistcomponent.o application/registry.o application/taggercomponent.o common/communication/application.o common/communication/joblist.o common/communication/menu.o common/communication/settings.o common/metadata/device.o common/metadata/format.o common/metadata/info.o common/metadata/mcdi.o common/metadata/picture.o common/metadata/picturedata.o common/metadata/track.o common/config.o common/configlayer.o common/i18n.o common/protocol.o common/utilities.o component/component.o component/decodercomponent.o component/deviceinfocomponent.o component/dspcomponent.o component/encodercomponent.o component/extensioncomponent.o component/outputcomponent.o component/playlistcomponent.o component/taggercomponent.o core/core.o core/main.o

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

include $(BOCA_PATH)/Makefile-directories

CC	  = gcc
CCOPTS	  = -g0 -Wall -Os -fno-exceptions -DBOCA_CDK_BUILD -DBOCA_INSTALL_PREFIX=\"$(PREFIX)\" -I$(BOCA_PATH)/include
LD	  = gcc
LDOPTS	  = -L$(BOCA_PATH)/$(LIBDIR) -lstdc++ -s $(LIBS)

ifeq ($(BUILD_X64),True)
	CCOPTS += -m64
	LDOPTS += -m64
else
	CCOPTS += -m32
	LDOPTS += -m32
endif

ifneq ($(BUILD_WIN32),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_HAIKU),True)
	CCOPTS += -pthread
endif
endif
endif
endif

ifeq ($(BUILD_WIN32),True)
	LDOPTS += -lsmooth -lws2_32 -Wl,--dynamicbase,--nxcompat,--enable-auto-import
else ifeq ($(BUILD_OSX),True)
	LDOPTS += -L$(PREFIX)/lib -lsmooth-$(SMOOTHVER) -Wl,-dylib_install_name,libboca-$(VERSION).0$(SHARED)
else
	LDOPTS += -L$(PREFIX)/lib -lsmooth-$(SMOOTHVER) -Wl,-soname,libboca-$(VERSION)$(SHARED).0 -Wl,-rpath,.

	ifeq ($(BUILD_SOLARIS),True)
		CCOPTS += -fPIC
	else ifeq ($(BUILD_LINUX),True)
		ifeq ($(BUILD_X64),True)
			CCOPTS += -fPIC
		endif
	endif
endif

all: $(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(SHARED)

$(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(SHARED): $(OBJECTS)
	mkdir -p $(BOCA_PATH)/$(BINDIR)
	mkdir -p $(BOCA_PATH)/$(LIBDIR)

ifeq ($(BUILD_WIN32),True)
	$(LD) --shared -o $@ $(OBJECTS) -Wl,--out-implib,$(BOCA_PATH)/$(LIBDIR)/libboca.a $(LDOPTS)
else
	$(LD) --shared -o $@ $(OBJECTS) $(LDOPTS)

	rm -f $(BOCA_PATH)/$(LIBDIR)/libboca-$(VERSION)$(SHARED)
	ln -s $(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(SHARED) $(BOCA_PATH)/$(LIBDIR)/libboca-$(VERSION)$(SHARED)
endif

clean:
ifeq ($(BUILD_WIN32),True)
	rm -f $(BOCA_PATH)/$(LIBDIR)/libboca.a $(OBJECTS)
else
	rm -f $(BOCA_PATH)/$(LIBDIR)/libboca-$(VERSION)$(SHARED)
endif

	rm -f $(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(SHARED) 

install: uninstall
ifneq ($(BUILD_WIN32),True)
	mkdir -p $(PREFIX)/$(LIB)/boca
	cp $(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(SHARED) $(PREFIX)/$(LIB)/boca
	ln -s $(PREFIX)/$(LIB)/boca/boca.$(VERSION)$(SHARED) $(PREFIX)/$(LIB)/libboca-$(VERSION)$(SHARED)

ifeq ($(BUILD_OSX),True)
	ln -s $(PREFIX)/$(LIB)/boca/boca.$(VERSION)$(SHARED) $(PREFIX)/$(LIB)/libboca-$(VERSION).0$(SHARED)
else
	ln -s $(PREFIX)/$(LIB)/boca/boca.$(VERSION)$(SHARED) $(PREFIX)/$(LIB)/libboca-$(VERSION)$(SHARED).0
endif

ifeq ($(BUILD_LINUX),True)
	ldconfig
else ifeq ($(BUILD_FREEBSD),True)
	ldconfig
else ifeq ($(BUILD_NETBSD),True)
	ldconfig
else ifeq ($(BUILD_HAIKU),True)
	ldconfig
endif
endif

uninstall:
ifneq ($(BUILD_WIN32),True)
ifeq ($(BUILD_OSX),True)
	rm -f $(PREFIX)/$(LIB)/libboca-$(VERSION).0$(SHARED)
else
	rm -f $(PREFIX)/$(LIB)/libboca-$(VERSION)$(SHARED).0
endif

	rm -f $(PREFIX)/$(LIB)/libboca-$(VERSION)$(SHARED)
	rm -f $(PREFIX)/$(LIB)/boca/boca.$(VERSION)$(SHARED)

ifeq ($(BUILD_LINUX),True)
	ldconfig
else ifeq ($(BUILD_FREEBSD),True)
	ldconfig
else ifeq ($(BUILD_NETBSD),True)
	ldconfig
else ifeq ($(BUILD_HAIKU),True)
	ldconfig
endif
endif

.c.o:
	$(CC) $(CCOPTS) -c $< -o $@

.cpp.o:
	$(CC) $(CCOPTS) -c $< -o $@
