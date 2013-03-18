########## BoCA component makefile ##########

# Change these variables to fit your project:
TARGET	  = donate
TYPE	  = extension
VERSION	  = 1.0

BOCA_PATH = ../../..

include $(BOCA_PATH)/Makefile-options

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
INSTCMD1  = cp -r boca.extension.donate $(PREFIX)/lib/boca
INSTCMD2  = chmod -R a=rX,u=rwX $(PREFIX)/lib/boca/boca.extension.donate
UINSTCMD1 = rm -f -r $(PREFIX)/lib/boca/boca.extension.donate
UINSTCMD2 =

## Do not change anything below this line. ##

include $(BOCA_PATH)/Makefile-directories
include $(BOCA_PATH)/Makefile-commands
