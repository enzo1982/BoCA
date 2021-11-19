BOCA_PATH = ../../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

OBJECTS	= ag_dec.o ALACBitUtilities.o ALACDecoder.o dp_dec.o EndianPortable.o matrix_dec.o
TARGET	= libalac.a

CCOPTS	= -Wno-multichar
AR	= ar

ifneq ($(BUILD_WIN32),True)
	CCOPTS += -fPIC
endif

OFLAGS	= -O3 -funroll-loops

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) rs $@ $(OBJECTS)

clean:
	rm -f $(TARGET) $(OBJECTS)

.c.o:
	$(CC) $(CCOPTS) $(CFLAGS) $(OFLAGS) -c $< -o $@


.cpp.o:
	$(CXX) $(CCOPTS) $(CXXFLAGS) $(OFLAGS) -c $< -o $@
