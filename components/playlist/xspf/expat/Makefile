BOCA_PATH = ../../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

OBJECTS	= xmlparse.o xmlrole.o xmltok.o
TARGET	= libexpat.a

CCOPTS	= -I"$(SRCDIR)" -DHAVE_EXPAT_CONFIG_H
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
