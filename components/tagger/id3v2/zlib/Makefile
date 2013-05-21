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

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(AR) cru $@ $(OBJECTS)

clean :
	rm -f $(TARGET) $(OBJECTS)

.c.o:
	$(CC) $(CCOPTS) -c $< -o $@
