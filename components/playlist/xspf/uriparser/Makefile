BOCA_PATH = ../../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

OBJECTS	= UriCommon.o UriCompare.o UriEscape.o UriFile.o UriIp4.o UriIp4Base.o UriNormalize.o UriNormalizeBase.o UriParse.o UriParseBase.o UriQuery.o UriRecompose.o UriResolve.o UriShorten.o
TARGET	= liburiparser.a

CC	= gcc
CCOPTS	= -I$(SRCDIR)/..
AR	= ar

ifeq ($(BUILD_FREEBSD),True)
	CC = clang
endif

ifneq ($(BUILD_WIN32),True)
	CCOPTS += -fPIC
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) rs $@ $(OBJECTS)

clean:
	rm -f $(TARGET) $(OBJECTS)

.c.o:
	$(CC) $(CCOPTS) $(CFLAGS) -c $< -o $@
