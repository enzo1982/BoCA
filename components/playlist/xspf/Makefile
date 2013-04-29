########## BoCA component makefile ##########

.NOTPARALLEL:

# Change these variables to fit your project:
TARGET	  = xspf
TYPE	  = playlist
VERSION	  = 1.0

BOCA_PATH = ../../..

# Enter object files here:
OBJECTS	  = xspf_list.o

# Enter additional defines here:
DEFINE	  = -I . -I expat

# Enter additional library dependencies here:
LIBS	  = xspf/libxspf.a uriparser/liburiparser.a expat/libexpat.a -lstdc++

# Enter addition commands for targets all and clean here:
ALLCMD1   = cd expat && $(MAKE) -j 4 && cd ../uriparser && $(MAKE) -j 4
ALLCMD2   = cd xspf && $(MAKE) -j 4
CLEANCMD1 = cd expat && $(MAKE) clean && cd ../uriparser && $(MAKE) clean
CLEANCMD2 = cd xspf && $(MAKE) clean
INSTCMD1  =
INSTCMD2  = 
UINSTCMD1 =
UINSTCMD2 =

## Do not change anything below this line. ##

include $(BOCA_PATH)/Makefile-directories
include $(BOCA_PATH)/Makefile-commands
