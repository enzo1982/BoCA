########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = faac
TYPE	  = encoder
VERSION	  = 1.0

BOCA_PATH = ../../..

# Enter object files here:
OBJECTS	  = config.o dllinterface.o faac_out.o

## Do not change anything below this line. ##

CC = gcc
CCOPTS = -march=i586 -Os -g0 -Wall -fno-exceptions -ffast-math -D__BOCA_COMPONENT_BUILD__ -I$(BOCA_PATH)/include
LD = gcc
LDOPTS = -L$(BOCA_PATH)/lib -lsmooth -lboca -lstdc++

all : $(BOCA_PATH)/bin/boca_$(TYPE)_$(TARGET).$(VERSION).dll

$(BOCA_PATH)/bin/boca_$(TYPE)_$(TARGET).$(VERSION).dll : $(OBJECTS)
	$(LD) --shared -o $@ $(OBJECTS) $(LDOPTS)
	strip --strip-all $@
	upx -9 -q $@

clean :
	rm -f $(BOCA_PATH)/bin/boca_$(TYPE)_$(TARGET).$(VERSION).dll $(OBJECTS)

.cpp.o:
	$(CC) $(CCOPTS) -c $< -o $@
