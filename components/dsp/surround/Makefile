########## BoCA component makefile ##########

.NOTPARALLEL:

# Change these variables to fit your project:
TARGET	  = surround
TYPE	  = dsp
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = surround.o config.o

# Enter additional defines here:
DEFINE	  = -I"$(SRCDIR)"/freesurround

# Enter additional library dependencies here:
LIBS	  = freesurround/libfreesurround.a

# Enter addition commands for targets all and clean here:
ALLCMD1   = $(call makein,freesurround)
ALLCMD2   =
CLEANCMD1 = $(call cleanin,freesurround)
CLEANCMD2 =
INSTCMD1  =
INSTCMD2  = 
UINSTCMD1 =
UINSTCMD2 =

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
