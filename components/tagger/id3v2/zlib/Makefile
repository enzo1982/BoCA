BOCA_PATH = ../../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

OBJECTS	= adler32.o compress.o crc32.o deflate.o infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o
TARGET	= libz.a

CCOPTS	= -I"$(SRCDIR)"
AR	= ar

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
