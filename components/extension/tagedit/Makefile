########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = tagedit
TYPE	  = extension
VERSION	  = 1.0

BOCA_PATH = ../../..

# Enter object files here:
OBJECTS	  = advanced/layer_edit_number.o advanced/layer_edit_text.o advanced/layer_tag_advanced.o basic/layer_tag_basic.o layer.o tagedit.o tagspec.o

## Do not change anything below this line. ##

CC = gcc
CCOPTS = -march=i586 -Os -g0 -Wall -fno-exceptions -ffast-math -D__BOCA_COMPONENT_BUILD__ -I$(BOCA_PATH)/include
LD = gcc
LDOPTS = -L$(BOCA_PATH)/lib -lsmooth -lboca -lstdc++

all : $(BOCA_PATH)/bin/boca_$(TYPE)_$(TARGET).$(VERSION).dll

$(BOCA_PATH)/bin/boca_$(TYPE)_$(TARGET).$(VERSION).dll : $(OBJECTS)
	$(LD) --shared -o $@ $(OBJECTS) $(LDOPTS)
	strip --strip-all $@
	upx --best -q $@

clean :
	rm -f $(BOCA_PATH)/bin/boca_$(TYPE)_$(TARGET).$(VERSION).dll $(OBJECTS)

.cpp.o:
	$(CC) $(CCOPTS) -c $< -o $@
