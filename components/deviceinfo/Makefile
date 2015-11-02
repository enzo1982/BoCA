########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

all:
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_OPENBSD),True)
	$(call makein,cdio)
endif
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_GNU),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	$(call makein,cdparanoia)
endif
endif
endif
endif
endif
endif
endif
ifeq ($(BUILD_WIN32),True)
	$(call makein,cdrip)
endif

clean:
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_OPENBSD),True)
	$(call cleanin,cdio)
endif
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_GNU),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	$(call cleanin,cdparanoia)
endif
endif
endif
endif
endif
endif
endif
ifeq ($(BUILD_WIN32),True)
	$(call cleanin,cdrip)
endif

install:
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_OPENBSD),True)
	$(call makein,cdio,install)
endif
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_GNU),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	$(call makein,cdparanoia,install)
endif
endif
endif
endif
endif
endif
endif

uninstall:
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_OPENBSD),True)
	$(call makein,cdio,uninstall)
endif
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_GNU),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	$(call makein,cdparanoia,uninstall)
endif
endif
endif
endif
endif
endif
endif
