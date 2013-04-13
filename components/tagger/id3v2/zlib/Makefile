OBJECTS	= adler32.o compress.o crc32.o deflate.o infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o
TARGET	= libz.a

CC = gcc
CCOPTS = -g0 -Os -fno-exceptions -I.
AR = ar

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(AR) cru $@ $(OBJECTS)
	strip --strip-debug $@

clean :
	rm -f $(TARGET) $(OBJECTS)

.c.o:
	$(CC) $(CCOPTS) -c $< -o $@
