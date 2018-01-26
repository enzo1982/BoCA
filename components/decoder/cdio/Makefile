########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = cdio
TYPE	  = decoder
VERSION	  = 1.0

BOCA_PATH = ../../..

# Enter object files here:
OBJECTS	  = cdio.o config.o info/cdinfo.o info/cdtext.o

# Enter additional defines here:
ifeq ($(wildcard /usr/include/cdio/paranoia/paranoia.h),)
ifneq ($(wildcard /usr/include/cdio/paranoia.h),)
	DEFINE = -DHAVE_OLD_PARANOIA_INCLUDE=1
endif
endif

# Enter additional library dependencies here:
LIBS	  = -lcdio -lcdio_cdda -lcdio_paranoia

ifeq ($(BUILD_WIN32),True)
	LIBS += -lwinmm
endif

# Enter addition commands for targets all and clean here:
ALLCMD1   =
ALLCMD2   =
CLEANCMD1 =
CLEANCMD2 =
INSTCMD1  =
INSTCMD2  = 
UINSTCMD1 =
UINSTCMD2 =

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
