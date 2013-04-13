OBJECTS	= xmlparse.o xmlrole.o xmltok.o
TARGET	= libexpat.a

CC = gcc
CCOPTS = -g0 -Os -fno-exceptions -I. -DHAVE_EXPAT_CONFIG_H
AR = ar

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(AR) cru $@ $(OBJECTS)
	strip --strip-debug $@

clean :
	rm -f $(TARGET) $(OBJECTS)

.c.o:
	$(CC) $(CCOPTS) -c $< -o $@
