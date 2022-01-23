BOCA_PATH = ../../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

OBJECTS	= channelmaps.o freesurround_decoder.o kiss_fft/kiss_fft.o kiss_fft/kiss_fftr.o
TARGET	= libfreesurround.a

CCOPTS	= -I"$(SRCDIR)"/kiss_fft -Dkiss_fft_scalar=double -Wno-unused-variable
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

.cpp.o:
	$(CC) $(CCOPTS) $(CXXFLAGS) -c $< -o $@
