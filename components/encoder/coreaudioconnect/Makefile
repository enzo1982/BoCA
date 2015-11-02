########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = coreaudioconnect
TYPE	  = encoder
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = config.o coreaudioconnect.o

# Enter additional defines here:
DEFINE	  = -Wno-multichar

# Enter additional library dependencies here:
LIBS	  =

# Enter addition commands for targets all and clean here:
ALLCMD1   = $(call makein,connector)
ALLCMD2   =
CLEANCMD1 = $(call cleanin,connector)
CLEANCMD2 =
INSTCMD1  = $(INSTALL) $(BOCA_PATH)/$(BINDIR)/boca_$(TYPE)_$(TARGET).$(VERSION)$(EXECUTABLE) $(DESTDIR)$(libdir)/boca
INSTCMD2  =
UINSTCMD1 = rm -f $(DESTDIR)$(libdir)/boca/boca_$(TYPE)_$(TARGET).$(VERSION)$(EXECUTABLE)
UINSTCMD2 =

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
