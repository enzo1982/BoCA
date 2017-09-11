BOCA_PATH = ..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

OBJECTS	= picturedata.o
TARGET	= picturedata$(EXECUTABLE)

LIBS	= -lboca -lsmooth -lstdc++

CCOPTS	= -I $(BOCA_PATH)/include
LDOPTS	= -L $(BOCA_PATH)/$(LIBDIR) $(LIBS)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) -o $@ $(OBJECTS) $(LDOPTS) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS)

.cpp.o:
	$(CXX) $(CCOPTS) $(CXXFLAGS) -c $< -o $@
