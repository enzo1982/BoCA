BOCA_PATH = ..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

TARGETS	= picturedata$(EXECUTABLE) multivalue$(EXECUTABLE)

LIBS	= -lboca -lsmooth

CCOPTS	= -I $(BOCA_PATH)/include
LDOPTS	= -L $(BOCA_PATH)/$(LIBDIR) $(LIBS)

all: $(TARGETS)

picturedata$(EXECUTABLE): picturedata.o
	$(LD) -o $@ picturedata.o $(LDOPTS) $(LDFLAGS)

multivalue$(EXECUTABLE): multivalue.o
	$(LD) -o $@ multivalue.o $(LDOPTS) $(LDFLAGS)

# On Windows, BoCA loads components only from the boca folder next to
# the program, so check copies the tagger components there.
ifeq ($(BUILD_WIN32),True)
CHECKCOMPONENTS = components
endif

check: $(CHECKCOMPONENTS) multivalue$(EXECUTABLE)
	./multivalue$(EXECUTABLE)

components:
	mkdir -p boca
	cp $(BOCA_PATH)/$(BINDIR)/boca_tagger_*$(SHARED) boca

clean:
	rm -f $(TARGETS) picturedata.o multivalue.o
	rm -rf boca

.PHONY: check components clean

.cpp.o:
	$(CXX) $(CCOPTS) $(CXXFLAGS) -c $< -o $@
