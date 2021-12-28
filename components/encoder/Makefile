########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

FOLDERS = fdkaac flac lame mac meh opus sndfile speex vorbis

ifneq ($(BUILD_OSX),True)
	FOLDERS += faac voaacenc
endif

ifeq ($(USE_WINE),True)
	FOLDERS += coreaudioconnect
endif

ifeq ($(BUILD_OSX),True)
	FOLDERS += coreaudio
else ifeq ($(BUILD_WIN32),True)
	FOLDERS += bladeenc bonk coreaudio wave wma

ifeq ($(BUILD_X86),True)
	FOLDERS += twinvq
else ifeq ($(BUILD_ARM64),True)
	FOLDERS += coreaudioconnect
endif
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
