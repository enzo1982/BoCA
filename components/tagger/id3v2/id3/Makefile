include ../../../../Makefile-options

OBJECTS	= field.o field_binary.o field_integer.o field_string_ascii.o field_string_unicode.o frame.o frame_impl.o frame_parse.o frame_render.o globals.o header.o header_frame.o header_tag.o helpers.o io.o io_decorators.o io_helpers.o misc_support.o mp3_parse.o readers.o spec.o tag.o tag_file.o tag_find.o tag_impl.o tag_parse.o tag_parse_lyrics3.o tag_parse_musicmatch.o tag_parse_v1.o tag_render.o utils.o writers.o
TARGET	= libid3.a

CC	= gcc
CCOPTS	= -g0 -Os -fno-exceptions -I. -I../zlib -DHAVE_CONFIG_H -DID3LIB_LINKOPTION=LINKOPTION_STATIC
AR	= ar

ifneq ($(BUILD_WIN32),True)
	CCOPTS += -fPIC
endif

ifeq ($(BUILD_OSX),True)
ifeq ($(BUILD_X86),True)
	CCOPTS += -arch i386
endif
ifeq ($(BUILD_X86_64),True)
	CCOPTS += -arch x86_64
endif
ifeq ($(BUILD_PPC),True)
	CCOPTS += -arch ppc
endif
ifeq ($(BUILD_PPC64),True)
	CCOPTS += -arch ppc64
endif
else ifeq ($(BUILD_X86),True)
	CCOPTS += -m32
else ifeq ($(BUILD_X86_64),True)
	CCOPTS += -m64
endif

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(AR) rs $@ $(OBJECTS)

clean :
	rm -f $(TARGET) $(OBJECTS)

.cpp.o:
	$(CC) $(CCOPTS) -c $< -o $@
