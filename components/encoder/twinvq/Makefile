########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = twinvq
TYPE	  = encoder
VERSION	  = 1.0

BOCA_PATH = ../../..

# Enter object files here:
OBJECTS	  = twinvq/bfile.o twinvq/bfile_e.o twinvq/bstream_e.o twinvq/Chunk.o twinvq/ChunkHelper.o config.o dllinterface.o twinvq.o

# Enter additional defines here:
DEFINE	  =

# Enter additional library dependencies here:
ifeq ($(BUILD_X86),True)
	LIBS = -lstdc++.static
endif

# Enter addition commands for targets all and clean here:
ALLCMD1   =
ALLCMD2   =
CLEANCMD1 =
CLEANCMD2 =
INSTCMD1  =
INSTCMD2  = 
UINSTCMD1 =
UINSTCMD2 =

# Additional compilation options
EXCEPTION = True

## Do not change anything below this line. ##

include $(BOCA_PATH)/Makefile-directories
include $(BOCA_PATH)/Makefile-commands
