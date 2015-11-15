BOCA_PATH = ../../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

OBJECTS	= XspfChunkCallback.o XspfData.o XspfDataWriter.o XspfDateTime.o XspfExtension.o XspfExtensionReader.o XspfExtensionReaderFactory.o XspfExtensionWriter.o XspfIndentFormatter.o XspfProps.o XspfPropsWriter.o XspfReader.o XspfReaderCallback.o XspfSeamlessFormatter.o XspfSkipExtensionReader.o XspfStrictReaderCallback.o XspfToolbox.o XspfTrack.o XspfTrackWriter.o XspfWriter.o XspfXmlFormatter.o
TARGET	= libxspf.a

CPPOPTS	= -I$(SRCDIR)/..
AR	= ar

ifeq ($(USE_BUNDLED_LIBEXPAT),True)
	CPPOPTS += -I$(SRCDIR)/../expat
endif

ifneq ($(BUILD_WIN32),True)
	CPPOPTS += -fPIC
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) rs $@ $(OBJECTS)

clean:
	rm -f $(TARGET) $(OBJECTS)

.cpp.o:
	$(CXX) $(CPPOPTS) $(CXXFLAGS) -c $< -o $@
