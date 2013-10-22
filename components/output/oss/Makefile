########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = oss
TYPE	  = output
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = oss.o

# Enter additional defines here:
DEFINE	  =

# Enter additional library dependencies here:
ifeq ($(BUILD_OPENBSD),True)
	LIBS	  = -lossaudio
else ifeq ($(BUILD_NETBSD),True)
	LIBS	  = -lossaudio
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
