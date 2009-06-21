########## BoCA CDK makefile ##########

# Change these variables to fit your project:
TARGET	  = boca
TYPE	  = runtime
VERSION	  = 1.0

BOCA_PATH = ..

# Enter object files here:
OBJECTS	  = application/external/configlayer.o application/external/decodercomponent.o application/external/encodercomponent.o application/component.o application/componentspecs.o application/decodercomponent.o application/dspcomponent.o application/encodercomponent.o application/extensioncomponent.o application/outputcomponent.o application/registry.o common/communication/joblist.o common/communication/menu.o common/tagging/tag.o common/tagging/tagape.o common/tagging/tagid3v1.o common/tagging/tagid3v2.o common/tagging/tagmp4.o common/tagging/tagvorbis.o common/track/format.o common/track/info.o common/track/picture.o common/track/track.o common/config.o common/configlayer.o common/i18n.o common/protocol.o common/utilities.o component/component.o component/decodercomponent.o component/dspcomponent.o component/encodercomponent.o component/extensioncomponent.o component/outputcomponent.o core/dllinterfaces.o core/main.o

ifeq ($(BUILD_WIN32),True)
	OBJECTS += application/external/decodercomponentfile.o application/external/decodercomponentstdio.o application/external/encodercomponentfile.o application/external/encodercomponentstdio.o common/tagging/tagwma.o
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
CCOPTS = -g0 -Wall -fno-exceptions -ffast-math -D__BOCA_CDK_BUILD__ -DID3LIB_LINKOPTION=LINKOPTION_USE_DYNAMIC -I$(BOCA_PATH)/include
LD = gcc
LDOPTS = -L$(BOCA_PATH)/$(LIBDIR) -lsmooth -lstdc++ $(LIBS)

ifeq ($(BUILD_X64),True)
	CCOPTS += -march=nocona

	PACKER = mpress
	PACKER_OPTS = -q
else
	CCOPTS += -march=i586 -Os

	PACKER = upx
	PACKER_OPTS = --best -q
endif

ifeq ($(BUILD_WIN32),True)
	LDOPTS += -lole32

	DLLEXT = .dll
else
	DLLEXT = .so
endif

all : $(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(DLLEXT)

$(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(DLLEXT) : $(OBJECTS)
ifeq ($(BUILD_WIN32),True)
	$(LD) --shared -o $@ $(OBJECTS) -Xlinker --out-implib -Xlinker $(BOCA_PATH)/$(LIBDIR)/libboca.a $(LDOPTS)
else
	$(LD) --shared -o $@ $(OBJECTS) $(LDOPTS)
	ln $(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(DLLEXT) $(BOCA_PATH)/$(LIBDIR)/libboca.so
endif
	strip --strip-all $@
#	$(PACKER) $(PACKER_OPTS) $@

clean :
	rm -f $(BOCA_PATH)/$(BINDIR)/boca.$(VERSION)$(DLLEXT) $(BOCA_PATH)/$(LIBDIR)/libboca.a $(OBJECTS)

.c.o:
	$(CC) $(CCOPTS) -c $< -o $@

.cpp.o:
	$(CC) $(CCOPTS) -c $< -o $@
