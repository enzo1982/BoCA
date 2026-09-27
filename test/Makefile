BOCA_PATH = ..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

TARGETS	= picturedata$(EXECUTABLE) discmetadata$(EXECUTABLE)
PLUGINS	= fakemetadata$(SHARED) minimalmetadata$(SHARED)

LIBS	= -lboca -lsmooth

CCOPTS	= -I $(BOCA_PATH)/include
LDOPTS	= -L $(BOCA_PATH)/$(LIBDIR) $(LIBS)

ifeq ($(BUILD_OSX),True)
	SHAREDOPTS = -dynamiclib
else
	SHAREDOPTS = --shared
endif

all: $(TARGETS) $(PLUGINS)

picturedata$(EXECUTABLE): picturedata.o
	$(LD) -o $@ picturedata.o $(LDOPTS) $(LDFLAGS)

discmetadata$(EXECUTABLE): discmetadata.o $(PLUGINS)
	$(LD) -o $@ discmetadata.o $(LDOPTS) $(LDFLAGS)

%$(SHARED): %.cpp
	$(CXX) $(CCOPTS) -fPIC -fvisibility=hidden -DBOCA_COMPONENT_BUILD $(CXXFLAGS) -c $< -o $*.pic.o
	$(LD) $(SHAREDOPTS) -o $@ $*.pic.o $(LDOPTS) $(LDFLAGS)

check: discmetadata$(EXECUTABLE)
	./discmetadata$(EXECUTABLE)

clean:
	rm -f $(TARGETS) $(PLUGINS) *.o

.PHONY: check clean

.cpp.o:
	$(CXX) $(CCOPTS) $(CXXFLAGS) -c $< -o $@
