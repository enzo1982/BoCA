include ../../../../Makefile-options

OBJECTS	= XspfChunkCallback.o XspfData.o XspfDataWriter.o XspfDateTime.o XspfExtension.o XspfExtensionReader.o XspfExtensionReaderFactory.o XspfExtensionWriter.o XspfIndentFormatter.o XspfProps.o XspfPropsWriter.o XspfReader.o XspfReaderCallback.o XspfSeamlessFormatter.o XspfSkipExtensionReader.o XspfStrictReaderCallback.o XspfToolbox.o XspfTrack.o XspfTrackWriter.o XspfWriter.o XspfXmlFormatter.o
TARGET	= libxspf.a

CPP	= gcc
CPPOPTS	= -g0 -Os -fno-exceptions -I.. -I../expat
AR	= ar

ifeq ($(BUILD_FREEBSD),True)
	CPP = clang
endif

ifneq ($(BUILD_WIN32),True)
	CPPOPTS += -fPIC
endif

ifeq ($(BUILD_OSX),True)
ifeq ($(BUILD_X86),True)
	CPPOPTS += -arch i386
endif
ifeq ($(BUILD_X86_64),True)
	CPPOPTS += -arch x86_64
endif
ifeq ($(BUILD_PPC),True)
	CPPOPTS += -arch ppc
endif
ifeq ($(BUILD_PPC64),True)
	CPPOPTS += -arch ppc64
endif
else ifeq ($(BUILD_X86),True)
	CPPOPTS += -m32
else ifeq ($(BUILD_X86_64),True)
	CPPOPTS += -m64
endif

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(AR) rs $@ $(OBJECTS)

clean :
	rm -f $(TARGET) $(OBJECTS)

.cpp.o:
	$(CPP) $(CPPOPTS) -c $< -o $@
