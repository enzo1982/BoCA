########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

FOLDERS = 

ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_HAIKU),True)
ifneq ($(BUILD_OPENBSD),True)
	FOLDERS += cdio
endif
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_GNU),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_NETBSD),True)
	FOLDERS += cdparanoia
endif
endif
endif
endif
endif
else ifeq ($(BUILD_WIN32),True)
	FOLDERS += cdrip
endif

.PHONY: $(FOLDERS)

all: $(FOLDERS)

$(FOLDERS):
	+ $(call makein,$@)

clean: $(foreach FOLDER,$(FOLDERS),$(FOLDER)##clean)

$(foreach FOLDER,$(FOLDERS),$(FOLDER)##clean):
	$(call cleanin,$(subst ##clean,,$@))

install: $(foreach FOLDER,$(FOLDERS),$(FOLDER)##install)

$(foreach FOLDER,$(FOLDERS),$(FOLDER)##install):
	$(call makein,$(subst ##install,,$@),install)

uninstall: $(foreach FOLDER,$(FOLDERS),$(FOLDER)##uninstall)

$(foreach FOLDER,$(FOLDERS),$(FOLDER)##uninstall):
	$(call makein,$(subst ##uninstall,,$@),uninstall)
