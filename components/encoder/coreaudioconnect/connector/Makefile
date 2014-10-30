BOCA_PATH = ../../../..

include $(BOCA_PATH)/Makefile-options
include $(BOCA_PATH)/Makefile-directories

OBJECTS	= dllinterface.o main.o
TARGET	= $(BOCA_PATH)/$(BINDIR)/boca_encoder_coreaudioconnect.1.0.exe

DEFINE	= -DUNICODE -D_UNICODE -Wno-multichar
LIBS	= -lole32

CC	= gcc
CCOPTS	= -g0 -Os -Wno-multichar -fno-exceptions -ffast-math -I . -I $(BOCA_PATH)/include $(DEFINE)
LD	= gcc
LDOPTS	= -L $(BOCA_PATH)/$(LIBDIR) $(LIBS)

ifeq ($(BUILD_FREEBSD),True)
	CC = clang
	LD = clang
endif

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(LD) -o $@ $(OBJECTS) $(LDOPTS)
	strip --strip-all $@

clean :
	rm -f $(TARGET) $(OBJECTS)

.cpp.o:
	$(CC) $(CCOPTS) -c $< -o $@
