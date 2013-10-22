########## BoCA component makefile ##########

.NOTPARALLEL:

# Change these variables to fit your project:
TARGET	  = id3v2
TYPE	  = tagger
VERSION	  = 1.0

BOCA_PATH = ../../..

# Enter object files here:
OBJECTS	  = id3v2.o

# Enter additional defines here:
DEFINE	  = -I id3 -DID3LIB_LINKOPTION=LINKOPTION_STATIC

# Enter additional library dependencies here:
LIBS	  = id3/libid3.a zlib/libz.a -lstdc++

# Enter addition commands for targets all and clean here:
ALLCMD1   = cd zlib && $(MAKE) -j 4
ALLCMD2   = cd id3 && $(MAKE) -j 4
CLEANCMD1 = cd zlib && $(MAKE) clean
CLEANCMD2 = cd id3 && $(MAKE) clean
INSTCMD1  =
INSTCMD2  = 
UINSTCMD1 =
UINSTCMD2 =

## Do not change anything below this line. ##

include $(BOCA_PATH)/Makefile-directories
include $(BOCA_PATH)/Makefile-commands
