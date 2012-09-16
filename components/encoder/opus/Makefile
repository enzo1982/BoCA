########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = opus
TYPE	  = encoder
VERSION	  = 1.0

BOCA_PATH = ../../..

# Enter object files here:
OBJECTS	  = config.o dllinterface.o opus_out.o

# Enter additional defines here:
ifeq ($(BUILD_WIN32),True)
DEFINE	  = -I .
endif

# Enter additional library dependencies here:
LIBS	  =

# Enter addition commands for targets all and clean here:
ALLCMD1   =
ALLCMD2   =
CLEANCMD1 =
CLEANCMD1 =

## Do not change anything below this line. ##

include $(BOCA_PATH)/Makefile-directories
include $(BOCA_PATH)/Makefile-commands
