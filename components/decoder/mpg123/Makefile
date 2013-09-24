########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = mpg123
TYPE	  = decoder
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = xing/dxhead.o config.o dllinterface.o mpg123_in.o

# Enter additional defines here:
DEFINE	  =

# Enter additional library dependencies here:
LIBS	  =

ifeq ($(BUILD_OSX),True)
	LIBS += -headerpad_max_install_names
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
