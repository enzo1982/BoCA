include ../../../../Makefile-options

OBJECTS	= XspfChunkCallback.o XspfData.o XspfDataWriter.o XspfDateTime.o XspfExtension.o XspfExtensionReader.o XspfExtensionReaderFactory.o XspfExtensionWriter.o XspfIndentFormatter.o XspfProps.o XspfPropsWriter.o XspfReader.o XspfReaderCallback.o XspfSeamlessFormatter.o XspfSkipExtensionReader.o XspfStrictReaderCallback.o XspfToolbox.o XspfTrack.o XspfTrackWriter.o XspfWriter.o XspfXmlFormatter.o
TARGET	= libxspf.a

CPP	= gcc
CPPOPTS	= -g0 -Os -fno-exceptions -I.. -I../expat
AR	= ar

ifeq ($(BUILD_SOLARIS),True)
	CPPOPTS += -fPIC
else ifeq ($(BUILD_LINUX),True)
	ifeq ($(BUILD_X86_64),True)
		CPPOPTS += -fPIC
	endif
endif

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(AR) cru $@ $(OBJECTS)

clean :
	rm -f $(TARGET) $(OBJECTS)

.cpp.o:
	$(CPP) $(CPPOPTS) -c $< -o $@
