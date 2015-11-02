########## BoCA component makefile ##########

.NOTPARALLEL:

# Change these variables to fit your project:
TARGET	  = xspf
TYPE	  = playlist
VERSION	  = 1.0

BOCA_PATH = ../../..

# Enter object files here:
OBJECTS	  = xspf.o

# Enter additional defines here:
DEFINE	  = -I$(SRCDIR) -I$(SRCDIR)/expat

# Enter additional library dependencies here:
LIBS	  = xspf/libxspf.a uriparser/liburiparser.a expat/libexpat.a -lstdc++

# Enter addition commands for targets all and clean here:
ALLCMD1   = $(call makein,expat) && $(call makein,uriparser)
ALLCMD2   = $(call makein,xspf)
CLEANCMD1 = $(call cleanin,expat) && $(call cleanin,uriparser)
CLEANCMD2 = $(call cleanin,xspf)
INSTCMD1  =
INSTCMD2  = 
UINSTCMD1 =
UINSTCMD2 =

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
