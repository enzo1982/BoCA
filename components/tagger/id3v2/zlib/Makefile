include ../../../../Makefile-options

OBJECTS	= adler32.o compress.o crc32.o deflate.o infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o
TARGET	= libz.a

CC	= gcc
CCOPTS	= -g0 -Os -fno-exceptions -I.
AR	= ar

ifeq ($(BUILD_SOLARIS),True)
	CCOPTS += -fPIC
else ifeq ($(BUILD_LINUX),True)
	ifeq ($(BUILD_X86_64),True)
		CCOPTS += -fPIC
	endif
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
