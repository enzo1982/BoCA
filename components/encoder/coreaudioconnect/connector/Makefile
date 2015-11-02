BOCA_PATH = ../../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

OBJECTS	= dllinterface.o main.o
TARGET	= $(BOCA_PATH)/$(BINDIR)/boca_encoder_coreaudioconnect.1.0$(EXECUTABLE)

DEFINE	= -DUNICODE -D_UNICODE
LIBS	= -lole32 -lshell32

CC	= gcc
CCOPTS	= -Wno-multichar -I$(SRCDIR) -I $(BOCA_PATH)/include $(DEFINE)
LD	= gcc
LDOPTS	= -L $(BOCA_PATH)/$(LIBDIR) $(LIBS)

ifneq ($(BUILD_WIN32),True)
	CC	= winegcc
	LD	= winegcc
endif

ifeq ($(BUILD_LINUX),True)
	LDOPTS += -lrt
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
ifeq ($(BUILD_WIN32),True)
	$(LD) -o $@ $(OBJECTS) $(LDOPTS) $(LDFLAGS)
else
	$(LD) -o $@$(SHARED) $(OBJECTS) $(LDOPTS) $(LDFLAGS)
	mv $@$(SHARED) $@
endif

clean:
	rm -f $(TARGET) $(OBJECTS)

.cpp.o:
	$(CC) $(CCOPTS) $(CXXFLAGS) -c $< -o $@
