include ../../../../Makefile-options

OBJECTS	= xmlparse.o xmlrole.o xmltok.o
TARGET	= libexpat.a

CC	= gcc
CCOPTS	= -g0 -Os -fno-exceptions -I. -DHAVE_EXPAT_CONFIG_H
AR	= ar

ifeq ($(BUILD_SOLARIS),True)
	CCOPTS += -fPIC
else ifeq ($(BUILD_LINUX),True)
	ifeq ($(BUILD_X64),True)
		CCOPTS += -fPIC
	endif
endif

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(AR) cru $@ $(OBJECTS)

clean :
	rm -f $(TARGET) $(OBJECTS)

.c.o:
	$(CC) $(CCOPTS) -c $< -o $@
