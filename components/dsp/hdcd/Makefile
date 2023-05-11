########## BoCA component makefile ##########

.NOTPARALLEL:

# Change these variables to fit your project:
TARGET	  = hdcd
TYPE	  = dsp
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = hdcd.o

# Enter additional defines here:
DEFINE	  = -I"$(SRCDIR)"/libhdcd

# Enter additional library dependencies here:
LIBS	  = libhdcd/libhdcd.a

# Enter addition commands for targets all and clean here:
ALLCMD1   = $(call makein,libhdcd)
ALLCMD2   =
CLEANCMD1 = $(call cleanin,libhdcd)
CLEANCMD2 =
INSTCMD1  =
INSTCMD2  = 
UINSTCMD1 =
UINSTCMD2 =

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
