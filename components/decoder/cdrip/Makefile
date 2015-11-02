########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = cdrip
TYPE	  = decoder
VERSION	  = 1.0

BOCA_PATH = ../../..

# Enter object files here:
OBJECTS	  = cdrip.o config.o dllinterface.o info/cdinfo.o info/cdplayerini.o info/cdtext.o

# Enter additional defines here:
DEFINE	  =

# Enter additional library dependencies here:
LIBS	  = -lws2_32

# Enter addition commands for targets all and clean here:
ALLCMD1   = mkdir -p info
ALLCMD2   =
CLEANCMD1 = rmdir info || true
CLEANCMD2 =
INSTCMD1  =
INSTCMD2  = 
UINSTCMD1 =
UINSTCMD2 =

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
