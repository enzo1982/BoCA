BOCA_PATH = ..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

TARGETS	= picturedata$(EXECUTABLE) multivalue$(EXECUTABLE) discmetadata$(EXECUTABLE)
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

multivalue$(EXECUTABLE): multivalue.o
	$(LD) -o $@ multivalue.o $(LDOPTS) $(LDFLAGS)

discmetadata$(EXECUTABLE): discmetadata.o $(PLUGINS)
	$(LD) -o $@ discmetadata.o $(LDOPTS) $(LDFLAGS)

%$(SHARED): %.cpp
	$(CXX) $(CCOPTS) -fPIC -fvisibility=hidden -DBOCA_COMPONENT_BUILD $(CXXFLAGS) -c $< -o $*.pic.o
	$(LD) $(SHAREDOPTS) -o $@ $*.pic.o $(LDOPTS) $(LDFLAGS)

# On Windows, BoCA loads components only from the boca folder next to
# the program, so check copies the tagger components there.
ifeq ($(BUILD_WIN32),True)
CHECKCOMPONENTS = components
endif

check: $(CHECKCOMPONENTS) multivalue$(EXECUTABLE) discmetadata$(EXECUTABLE)
	./multivalue$(EXECUTABLE)
	./discmetadata$(EXECUTABLE)

components:
	mkdir -p boca
	cp $(BOCA_PATH)/$(BINDIR)/boca_tagger_*$(SHARED) boca

clean:
	rm -f $(TARGETS) $(PLUGINS) *.o
	rm -rf boca

.PHONY: check components clean

.cpp.o:
	$(CXX) $(CCOPTS) $(CXXFLAGS) -c $< -o $@
