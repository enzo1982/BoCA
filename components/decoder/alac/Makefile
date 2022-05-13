########## BoCA component makefile ##########

.NOTPARALLEL:

# Change these variables to fit your project:
TARGET	  = alac
TYPE	  = decoder
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = dllinterface.o alac.o

# Enter additional defines here:
DEFINE	  = -I"$(SRCDIR)" -I"$(SRCDIR)"/$(BOCA_PATH)/include/support -Wno-multichar

# Enter additional library dependencies here:
LIBS	  = alac/libalac.a

# Enter addition commands for targets all and clean here:
ALLCMD1   = $(call makein,alac)
ALLCMD2   =
CLEANCMD1 = $(call cleanin,alac)
CLEANCMD2 =
INSTCMD1  =
INSTCMD2  = 
UINSTCMD1 =
UINSTCMD2 =

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
