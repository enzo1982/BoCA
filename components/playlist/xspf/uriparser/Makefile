include ../../../../Makefile-options

OBJECTS	= UriCommon.o UriCompare.o UriEscape.o UriFile.o UriIp4.o UriIp4Base.o UriNormalize.o UriNormalizeBase.o UriParse.o UriParseBase.o UriQuery.o UriRecompose.o UriResolve.o UriShorten.o
TARGET	= liburiparser.a

CC	= gcc
CCOPTS	= -g0 -Os -fno-exceptions -I..
AR	= ar

ifeq ($(BUILD_FREEBSD),True)
	CC = clang
endif

ifneq ($(BUILD_WIN32),True)
	CCOPTS += -fPIC
endif

ifeq ($(BUILD_OSX),True)
ifeq ($(BUILD_X86),True)
	CCOPTS += -arch i386
endif
ifeq ($(BUILD_X86_64),True)
	CCOPTS += -arch x86_64
endif
ifeq ($(BUILD_PPC),True)
	CCOPTS += -arch ppc
endif
ifeq ($(BUILD_PPC64),True)
	CCOPTS += -arch ppc64
endif
else ifeq ($(BUILD_X86),True)
	CCOPTS += -m32
else ifeq ($(BUILD_X86_64),True)
	CCOPTS += -m64
endif

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(AR) rs $@ $(OBJECTS)

clean :
	rm -f $(TARGET) $(OBJECTS)

.c.o:
	$(CC) $(CCOPTS) -c $< -o $@
