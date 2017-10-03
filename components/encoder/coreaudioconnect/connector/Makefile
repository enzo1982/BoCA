BOCA_PATH = ../../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

ifeq ($(BUILD_X86_64),True)
	X64 = 64
endif

OBJECTS	= dllinterface.o main.o
TARGET	= $(BOCA_PATH)/$(BINDIR)/boca_encoder_coreaudioconnect$(X64).1.0$(EXECUTABLE)

DEFINE	= -DUNICODE
LIBS	= -lole32 -lshell32

CCOPTS	= -Wno-multichar -I$(SRCDIR) -I $(BOCA_PATH)/include $(DEFINE)
LDOPTS	= -L $(BOCA_PATH)/$(LIBDIR) $(LIBS)

ifneq ($(BUILD_WIN32),True)
	override CXX = winegcc
	override LD  = winegcc
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
	$(CXX) $(CCOPTS) $(CXXFLAGS) -c $< -o $@
