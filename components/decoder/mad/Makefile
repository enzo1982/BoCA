########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = mad
TYPE	  = decoder
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = xing/dxhead.o config.o dllinterface.o mad.o

# Enter additional defines here:
DEFINE	  = -I"$(SRCDIR)"/$(BOCA_PATH)/include/support

# Enter additional library dependencies here:
LIBS	  =

# Enter addition commands for targets all and clean here:
ifneq ("$(SRCDIR)","$(CURDIR)")
	ALLCMD1   = mkdir -p xing
	ALLCMD2   =
	CLEANCMD1 = rmdir xing || true
	CLEANCMD2 =
	INSTCMD1  =
	INSTCMD2  = 
	UINSTCMD1 =
	UINSTCMD2 =
endif

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
