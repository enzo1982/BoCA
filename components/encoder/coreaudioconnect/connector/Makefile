########## Core Audio connector makefile ##########

# Change these variables to fit your project:
TARGET	  = coreaudioconnect
TYPE	  = encoder
VERSION	  = 1.0

BOCA_PATH = ../../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

ifeq ($(BUILD_X86_64),True)
	X64 = 64
endif

OBJECTS	= dllinterface.o main.o
BINARY	= $(BOCA_PATH)/$(BINDIR)/boca_$(TYPE)_$(TARGET)$(X64).$(VERSION)$(EXECUTABLE)

DEFINE	= -DUNICODE
LIBS	= -lole32 -lshell32

CCOPTS	= -Wno-multichar -I"$(SRCDIR)"/$(BOCA_PATH)/include -I"$(SRCDIR)"/$(BOCA_PATH)/include/support/apple $(DEFINE)
LDOPTS	= -L $(BOCA_PATH)/$(LIBDIR) $(LIBS)

ifneq ($(BUILD_WIN32),True)
	override CXX = winegcc
	override LD  = winegcc
endif

ifeq ($(BUILD_LINUX),True)
	LDOPTS += -lrt
endif

ifeq ($(BUILD_X86),True)
	CCOPTS += -m32
	LDOPTS += -m32
endif

all: $(BINARY)

$(BINARY): $(OBJECTS)
ifeq ($(BUILD_WIN32),True)
	$(LD) -o $@ $(OBJECTS) $(LDOPTS) $(LDFLAGS)
else
	$(LD) -o $@$(SHARED) $(OBJECTS) $(LDOPTS) $(LDFLAGS)
	mv $@$(SHARED) $@
endif

clean:
	rm -f $(BINARY) $(OBJECTS)

install: all
ifneq ($(BUILD_WIN32),True)
	$(INSTALL) -d "$(DESTDIR)"$(libdir)/boca
	$(INSTALL_DATA) $(BOCA_PATH)/$(BINDIR)/boca_$(TYPE)_$(TARGET)$(X64).$(VERSION)$(EXECUTABLE) "$(DESTDIR)"$(libdir)/boca
endif

uninstall:
ifneq ($(BUILD_WIN32),True)
	rm -f "$(DESTDIR)"$(libdir)/boca/boca_$(TYPE)_$(TARGET)$(X64).$(VERSION)$(EXECUTABLE)
endif

.cpp.o:
	$(CXX) $(CCOPTS) $(CXXFLAGS) -c $< -o $@
