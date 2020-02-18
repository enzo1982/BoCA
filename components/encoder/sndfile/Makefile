########## BoCA component makefile ##########

# Change these variables to fit your project:
ifeq ($(wrapper),)
	TARGET	  = sndfile
else
	TARGET	  = sndfile_$(wrapper)
endif

TYPE	  = encoder
VERSION	  = 1.0

BOCA_PATH = ../../..

# Enter object files here:
ifeq ($(wrapper),)
	OBJECTS	  = config.o dllinterface.o sndfile.o
else
	OBJECTS	  = config-$(wrapper).o dllinterface.o sndfile-$(wrapper).o
endif

# Enter additional defines here:
DEFINE	  =

# Enter additional library dependencies here:
LIBS	  =

# Enter addition commands for targets all and clean here:
ifeq ($(wrapper),)
	ALLCMD1   = $(MAKE) -f "$(SRCDIR)"/Makefile config=$(config) wrapper=wave
	ALLCMD2   =
	CLEANCMD1 = $(MAKE) -f "$(SRCDIR)"/Makefile config=$(config) wrapper=wave clean
	CLEANCMD2 =
	INSTCMD1  = $(MAKE) -f "$(SRCDIR)"/Makefile config=$(config) wrapper=wave install
	INSTCMD2  = 
	UINSTCMD1 = $(MAKE) -f "$(SRCDIR)"/Makefile config=$(config) wrapper=wave uninstall
	UINSTCMD2 =
else
	ALLCMD1   =
	ALLCMD2   =
	CLEANCMD1 =
	CLEANCMD2 =
	INSTCMD1  =
	INSTCMD2  = 
	UINSTCMD1 =
	UINSTCMD2 =
endif

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
