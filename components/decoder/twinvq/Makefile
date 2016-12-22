########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = twinvq
TYPE	  = decoder
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = twinvq/bfile.o twinvq/bstream.o twinvq/Chunk.o twinvq/HeaderManager.o dllinterface.o twinvq.o

# Enter additional defines here:
DEFINE	  =

# Enter additional library dependencies here:
ifeq ($(BUILD_X86),True)
	LIBS = -lstdc++.static
endif

# Enter addition commands for targets all and clean here:
ifneq ($(SRCDIR),$(CURDIR))
	ALLCMD1   = mkdir -p twinvq
	ALLCMD2   =
	CLEANCMD1 = rmdir twinvq || true
	CLEANCMD2 =
	INSTCMD1  =
	INSTCMD2  = 
	UINSTCMD1 =
	UINSTCMD2 =
endif

# Additional compilation options
EXCEPTION = True

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
