BOCA_PATH = ../../../..

include $(BOCA_PATH)/Makefile-options
include $(BOCA_PATH)/Makefile-directories

OBJECTS	= dllinterface.o main.o
TARGET	= $(BOCA_PATH)/$(BINDIR)/boca_encoder_coreaudioconnect.1.0$(EXECUTABLE)

DEFINE	= -DUNICODE -D_UNICODE -Wno-multichar
LIBS	= -lole32 -lshell32

CC	= gcc
CCOPTS	= -g0 -Os -Wno-multichar -fno-exceptions -ffast-math -I . -I $(BOCA_PATH)/include $(DEFINE)
LD	= gcc
LDOPTS	= -L $(BOCA_PATH)/$(LIBDIR) $(LIBS)

ifneq ($(BUILD_WIN32),True)
	CC = winegcc
	LD = winegcc
endif

ifeq ($(BUILD_LINUX),True)
	LDOPTS += -lrt
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
ifeq ($(BUILD_WIN32),True)
	$(LD) -o $@ $(OBJECTS) $(LDOPTS)
	strip --strip-all $@
else
	$(LD) -o $@$(SHARED) $(OBJECTS) $(LDOPTS)
	mv $@$(SHARED) $@
endif

clean:
	rm -f $(TARGET) $(OBJECTS)

.cpp.o:
	$(CC) $(CCOPTS) -c $< -o $@
