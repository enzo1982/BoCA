########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = youtube
TYPE	  = extension
VERSION	  = 1.0
SMOOTHVER = 0.8.71

BOCA_PATH = ../../..

include $(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = config.o converter.o layer.o scriptutilities.o video.o videolist.o videolistentry.o videosite.o youtube.o

# Enter additional defines here:
DEFINE	  = -DV8_USE_UNSAFE_HANDLES

# Enter additional library dependencies here:
ifeq ($(BUILD_WIN32),True)
	LIBS	  = -lsmooth-js
else
	LIBS	  = -lsmooth-js-$(SMOOTHVER)
endif

# Enter addition commands for targets all and clean here:
ALLCMD1   =
ALLCMD2   =
CLEANCMD1 =
CLEANCMD2 =
INSTCMD1  = cp -r boca.extension.youtube $(PREFIX)/$(LIB)/boca
INSTCMD2  = chmod -R a=rX,u=rwX $(PREFIX)/$(LIB)/boca/boca.extension.youtube
UINSTCMD1 = rm -f -r $(PREFIX)/$(LIB)/boca/boca.extension.youtube
UINSTCMD2 =

## Do not change anything below this line. ##

include $(BOCA_PATH)/Makefile-directories
include $(BOCA_PATH)/Makefile-commands
