########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = waveout
TYPE	  = output
VERSION	  = 1.0

BOCA_PATH = ../../..

# Enter object files here:
OBJECTS	  = waveout.o

# Enter additional defines here:
ifeq ($(BUILD_WIN32),True)
	DEFINE	= -DUNICODE
endif

# Enter additional library dependencies here:
LIBS	  = -lwinmm -lksguid

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
