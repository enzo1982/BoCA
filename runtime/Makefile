########## BoCA CDK makefile ##########

# Change these variables to fit your project:
TARGET	  = boca
TYPE	  = runtime
VERSION	  = 1.0

BOCA_PATH = ..

include $(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = application/external/configlayer.o application/external/decodercomponent.o application/external/encodercomponent.o application/component.o application/componentspecs.o application/decodercomponent.o application/deviceinfocomponent.o application/dspcomponent.o application/encodercomponent.o application/extensioncomponent.o application/outputcomponent.o application/registry.o application/taggercomponent.o common/communication/application.o common/communication/joblist.o common/communication/menu.o common/communication/settings.o common/metadata/device.o common/metadata/format.o common/metadata/info.o common/metadata/mcdi.o common/metadata/picture.o common/metadata/picturedata.o common/metadata/track.o common/config.o common/configlayer.o common/i18n.o common/protocol.o common/utilities.o component/component.o component/decodercomponent.o component/deviceinfocomponent.o component/dspcomponent.o component/encodercomponent.o component/extensioncomponent.o component/outputcomponent.o component/taggercomponent.o core/core.o core/main.o

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

CC = gcc
CCOPTS = -g0 -Wall -Os -ffast-math -fno-exceptions -DBOCA_CDK_BUILD -I$(BOCA_PATH)/include
LD = gcc
LDOPTS = -L$(BOCA_PATH)/$(LIBDIR) -lsmooth -lstdc++ $(LIBS)

ifeq ($(BUILD_X64),True)
	CCOPTS += -m64 -march=nocona
	LDOPTS += -m64
else
	CCOPTS += -m32 -march=pentium4
	LDOPTS += -m32
endif

ifeq ($(BUILD_WIN32),True)
	DLLEXT = .dll

	LDOPTS += -lws2_32 -Wl,--dynamicbase,--nxcompat,--enable-auto-import
else ifeq ($(BUILD_OSX),True)
	DLLEXT = .dylib

	LDOPTS += -Wl,-dylib_install_name,boca.$(VERSION)$(DLLEXT)
else
	DLLEXT = .so

ifeq ($(BUILD_SOLARIS),True)
	CCOPTS += -fpic
endif
endif

all : $(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(DLLEXT)

$(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(DLLEXT) : $(OBJECTS)
ifeq ($(BUILD_WIN32),True)
	$(LD) --shared -o $@ $(OBJECTS) -Wl,--out-implib,$(BOCA_PATH)/$(LIBDIR)/libboca.a $(LDOPTS)
else
	$(LD) --shared -o $@ $(OBJECTS) $(LDOPTS)
	rm -f $(BOCA_PATH)/$(LIBDIR)/libboca$(DLLEXT)
	ln -s $(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(DLLEXT) $(BOCA_PATH)/$(LIBDIR)/libboca$(DLLEXT)
endif
ifneq ($(BUILD_OSX),True)
	strip --strip-all $@
endif

clean :
	rm -f $(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(DLLEXT) $(BOCA_PATH)/$(LIBDIR)/libboca.a $(OBJECTS)

.c.o:
	$(CC) $(CCOPTS) -c $< -o $@

.cpp.o:
	$(CC) $(CCOPTS) -c $< -o $@
