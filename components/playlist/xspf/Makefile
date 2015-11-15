########## BoCA component makefile ##########

.NOTPARALLEL:

# Change these variables to fit your project:
TARGET	  = xspf
TYPE	  = playlist
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = xspf.o

# Enter additional defines here:
DEFINE	  =

ifeq ($(USE_BUNDLED_LIBEXPAT),True)
DEFINE	  += -I$(SRCDIR)/expat
endif

ifeq ($(USE_BUNDLED_LIBURIPARSER),True)
DEFINE	  += -I$(SRCDIR)
endif

ifeq ($(USE_BUNDLED_LIBXSPF),True)
DEFINE	  += -I$(SRCDIR)
endif

# Enter additional library dependencies here:
LIBS	  = -lstdc++

ifeq ($(USE_BUNDLED_LIBEXPAT),True)
LIBS	  += expat/libexpat.a
else
LIBS	  += -lexpat
endif

ifeq ($(USE_BUNDLED_LIBURIPARSER),True)
LIBS	  += uriparser/liburiparser.a
else
LIBS	  += -luriparser
endif

ifeq ($(USE_BUNDLED_LIBXSPF),True)
LIBS	  += xspf/libxspf.a
else
LIBS	  += -lxspf
endif

# Enter addition commands for targets all and clean here:
ALLCMD1   =
ALLCMD2   =
CLEANCMD1 =
CLEANCMD2 =

ifeq ($(USE_BUNDLED_LIBEXPAT),True)
ALLCMD1	  += $(call makein,expat)
CLEANCMD1 += $(call cleanin,expat)
endif

ifeq ($(USE_BUNDLED_LIBURIPARSER),True)
ifneq ($(ALLCMD1),)
ALLCMD1	  += &&
CLEANCMD1 += &&
endif

ALLCMD1	  += $(call makein,uriparser)
CLEANCMD1 += $(call cleanin,uriparser)
endif

ifeq ($(USE_BUNDLED_LIBXSPF),True)
ifneq ($(ALLCMD1),)
ALLCMD1	  += &&
CLEANCMD1 += &&
endif

ALLCMD1	  += $(call makein,xspf)
CLEANCMD1 += $(call cleanin,xspf)
endif

INSTCMD1  =
INSTCMD2  = 
UINSTCMD1 =
UINSTCMD2 =

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
