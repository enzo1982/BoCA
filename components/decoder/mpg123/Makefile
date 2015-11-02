########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = mpg123
TYPE	  = decoder
VERSION	  = 1.0

BOCA_PATH = ../../..

# Enter object files here:
OBJECTS	  = xing/dxhead.o config.o dllinterface.o mpg123.o

# Enter additional defines here:
DEFINE	  =

# Enter additional library dependencies here:
LIBS	  =

# Enter addition commands for targets all and clean here:
ALLCMD1   = mkdir -p xing
ALLCMD2   =
CLEANCMD1 = rmdir xing || true
CLEANCMD2 =
INSTCMD1  =
INSTCMD2  = 
UINSTCMD1 =
UINSTCMD2 =

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
