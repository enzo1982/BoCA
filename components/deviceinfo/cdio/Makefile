########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = cdio
TYPE	  = deviceinfo
VERSION	  = 1.0

BOCA_PATH = ../../..

# Enter object files here:
OBJECTS	  = cdio.o

# Enter additional defines here:
DEFINE	  =

# Enter additional library dependencies here:
LIBS	  = -lcdio

ifeq ($(BUILD_WIN32),True)
	LIBS += -lws2_32 -lwinmm
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

include $(BOCA_PATH)/Makefile-directories
include $(BOCA_PATH)/Makefile-commands
