BOCA_PATH = ../../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

OBJECTS	= hdcd_analyze_tonegen.o hdcd_decode2.o hdcd_libversion.o hdcd_simple.o hdcd_strings.o
TARGET	= libhdcd.a

CCOPTS	=
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
