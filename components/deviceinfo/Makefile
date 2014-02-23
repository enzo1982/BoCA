########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(BOCA_PATH)/Makefile-options

all:
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_OPENBSD),True)
	cd cdio && $(MAKE)
endif
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_GNU),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	cd cdparanoia && $(MAKE)
endif
endif
endif
endif
endif
endif
endif
ifeq ($(BUILD_WIN32),True)
	cd cdrip && $(MAKE)
endif

clean:
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_OPENBSD),True)
	cd cdio && $(MAKE) clean
endif
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_GNU),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	cd cdparanoia && $(MAKE) clean
endif
endif
endif
endif
endif
endif
endif
ifeq ($(BUILD_WIN32),True)
	cd cdrip && $(MAKE) clean
endif

install:
ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_QNX),True)
ifneq ($(BUILD_OPENBSD),True)
	cd cdio && $(MAKE) install
endif
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_GNU),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	cd cdparanoia && $(MAKE) install
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
	cd cdio && $(MAKE) uninstall
endif
ifneq ($(BUILD_OSX),True)
ifneq ($(BUILD_GNU),True)
ifneq ($(BUILD_SOLARIS),True)
ifneq ($(BUILD_NETBSD),True)
ifneq ($(BUILD_HAIKU),True)
	cd cdparanoia && $(MAKE) uninstall
endif
endif
endif
endif
endif
endif
endif
