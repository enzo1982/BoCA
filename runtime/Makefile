########## BoCA CDK makefile ##########

VERSION	   = 1.0

SRCDIR	   = src
INCLUDEDIR = ../include
LIBDIR	   = ../lib
BINDIR	   = ../bin
OBJECTSDIR = objects

OBJECTS	   = $(OBJECTSDIR)/application/component.o $(OBJECTSDIR)/application/componentspecs.o $(OBJECTSDIR)/application/decodercomponent.o $(OBJECTSDIR)/application/encodercomponent.o $(OBJECTSDIR)/application/encodercomponentexternal.o $(OBJECTSDIR)/application/outputcomponent.o $(OBJECTSDIR)/application/registry.o $(OBJECTSDIR)/common/tagging/tag.o $(OBJECTSDIR)/common/tagging/tagape.o $(OBJECTSDIR)/common/tagging/tagid3.o $(OBJECTSDIR)/common/config.o $(OBJECTSDIR)/common/configlayer.o $(OBJECTSDIR)/common/i18n.o $(OBJECTSDIR)/common/picture.o $(OBJECTSDIR)/common/track.o $(OBJECTSDIR)/common/utilities.o $(OBJECTSDIR)/component/component.o $(OBJECTSDIR)/component/decodercomponent.o $(OBJECTSDIR)/component/encodercomponent.o $(OBJECTSDIR)/component/outputcomponent.o $(OBJECTSDIR)/core/dllinterfaces.o $(OBJECTSDIR)/core/main.o

CC = gcc
CCOPTS = -march=i586 -Os -g0 -Wall -fno-exceptions -ffast-math -D__BOCA_CDK_BUILD__ -DID3LIB_LINKOPTION=LINKOPTION_USE_DYNAMIC -I$(INCLUDEDIR)
LD = gcc
LDOPTS = -L$(LIBDIR) -lsmooth -lstdc++

all : $(BINDIR)/boca.$(VERSION).dll

$(BINDIR)/boca.$(VERSION).dll : $(OBJECTS)
	$(LD) --shared -o $(BINDIR)/boca.$(VERSION).dll $(OBJECTS) -Xlinker --out-implib -Xlinker $(LIBDIR)/libboca.a $(LDOPTS)
	strip --strip-all $(BINDIR)/boca.$(VERSION).dll
	upx -9 -q $(BINDIR)/boca.$(VERSION).dll

clean :
	rm -f $(BINDIR)/boca.$(VERSION).dll $(LIBDIR)/libboca.a $(OBJECTS)

$(OBJECTSDIR)/application/%.o: $(SRCDIR)/application/%.cpp
	$(CC) $(CCOPTS) -c $< -o $@

$(OBJECTSDIR)/common/%.o: $(SRCDIR)/common/%.cpp
	$(CC) $(CCOPTS) -c $< -o $@

$(OBJECTSDIR)/common/tagging/%.o: $(SRCDIR)/common/tagging/%.cpp
	$(CC) $(CCOPTS) -c $< -o $@

$(OBJECTSDIR)/component/%.o: $(SRCDIR)/component/%.cpp
	$(CC) $(CCOPTS) -c $< -o $@

$(OBJECTSDIR)/core/%.o: $(SRCDIR)/core/%.cpp
	$(CC) $(CCOPTS) -c $< -o $@
