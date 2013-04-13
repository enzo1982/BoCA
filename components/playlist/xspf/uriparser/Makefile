OBJECTS	= UriCommon.o UriCompare.o UriEscape.o UriFile.o UriIp4.o UriIp4Base.o UriNormalize.o UriNormalizeBase.o UriParse.o UriParseBase.o UriQuery.o UriRecompose.o UriResolve.o UriShorten.o
TARGET	= liburiparser.a

CC = gcc
CCOPTS = -g0 -Os -Wno-attributes -fno-exceptions -I..
AR = ar

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(AR) cru $@ $(OBJECTS)
	strip --strip-debug $@

clean :
	rm -f $(TARGET) $(OBJECTS)

.c.o:
	$(CC) $(CCOPTS) -c $< -o $@
