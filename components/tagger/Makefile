########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

FOLDERS = apev2 cart flac id3v1 id3v2 mp4 riff vorbis

ifeq ($(BUILD_OSX),True)
	FOLDERS += tocplist
else ifeq ($(BUILD_WIN32),True)
	FOLDERS += wma
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
