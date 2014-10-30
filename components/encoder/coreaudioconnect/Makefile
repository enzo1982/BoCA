########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = coreaudioconnect
TYPE	  = encoder
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = config.o coreaudioconnect.o

# Enter additional defines here:
DEFINE	  = -Wno-multichar

# Enter additional library dependencies here:
LIBS	  =

# Enter addition commands for targets all and clean here:
ALLCMD1   = cd connector && $(MAKE) -j 4
ALLCMD2   =
CLEANCMD1 = cd connector && $(MAKE) clean
CLEANCMD2 =
INSTCMD1  =
INSTCMD2  = 
UINSTCMD1 =
UINSTCMD2 =

## Do not change anything below this line. ##

include $(BOCA_PATH)/Makefile-directories
include $(BOCA_PATH)/Makefile-commands
