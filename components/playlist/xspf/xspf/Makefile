OBJECTS	= XspfChunkCallback.o XspfData.o XspfDataWriter.o XspfDateTime.o XspfExtension.o XspfExtensionReader.o XspfExtensionReaderFactory.o XspfExtensionWriter.o XspfIndentFormatter.o XspfProps.o XspfPropsWriter.o XspfReader.o XspfReaderCallback.o XspfSeamlessFormatter.o XspfSkipExtensionReader.o XspfStrictReaderCallback.o XspfToolbox.o XspfTrack.o XspfTrackWriter.o XspfWriter.o XspfXmlFormatter.o
TARGET	= libxspf.a

CPP = gcc
CPPOPTS = -g0 -Os -fno-exceptions -I.. -I../expat
AR = ar

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(AR) cru $@ $(OBJECTS)
	strip --strip-debug $@

clean :
	rm -f $(TARGET) $(OBJECTS)

.cpp.o:
	$(CPP) $(CPPOPTS) -c $< -o $@
