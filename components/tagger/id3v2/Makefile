########## BoCA component makefile ##########

.NOTPARALLEL:

# Change these variables to fit your project:
TARGET	  = id3v2
TYPE	  = tagger
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = id3v2.o

# Enter additional defines here:
DEFINE	  = -I"$(SRCDIR)"/id3 -DID3LIB_LINKOPTION=LINKOPTION_STATIC

# Enter additional library dependencies here:
LIBS	  = id3/libid3.a -lstdc++

ifeq ($(USE_BUNDLED_ZLIB),True)
LIBS	  += zlib/libz.a
else
LIBS	  += -lz
endif

# Enter addition commands for targets all and clean here:
ALLCMD1   = $(call makein,id3)
ALLCMD2   =
CLEANCMD1 = $(call cleanin,id3)
CLEANCMD2 =

ifeq ($(USE_BUNDLED_ZLIB),True)
ALLCMD1	  += && $(call makein,zlib)
CLEANCMD1 += && $(call cleanin,zlib)
endif

INSTCMD1  =
INSTCMD2  = 
UINSTCMD1 =
UINSTCMD2 =

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
