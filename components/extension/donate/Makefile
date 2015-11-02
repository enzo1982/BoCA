########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = donate
TYPE	  = extension
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = dialog.o donate.o

# Enter additional defines here:
DEFINE	  =

# Enter additional library dependencies here:
LIBS	  =

# Enter addition commands for targets all and clean here:
ALLCMD1   =
ALLCMD2   =
CLEANCMD1 =
CLEANCMD2 =
INSTCMD1  = cp -r $(SRCDIR)/boca.extension.donate $(DESTDIR)$(libdir)/boca
INSTCMD2  = chmod -R a=rX,u=rwX $(DESTDIR)$(libdir)/boca/boca.extension.donate
UINSTCMD1 = rm -f -r $(DESTDIR)$(libdir)/boca/boca.extension.donate
UINSTCMD2 =

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-commands
