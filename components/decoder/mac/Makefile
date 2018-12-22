########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = mac
TYPE	  = decoder
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options 

# Enter object files here:
OBJECTS	  = dllinterface.o mac.o

# Enter additional defines here:
ifeq ($(BUILD_WIN32),True)
	DEFINE = -DPLATFORM_WINDOWS
else
	DEFINE = -DPLATFORM_LINUX
endif

# Enter additional library dependencies here:
LIBS	  =

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
