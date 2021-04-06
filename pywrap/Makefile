CXX=g++
LD=g++

CXXFLAGS += -Wall -O2 -std=c++11 -g -fPIC $(shell python3 -m pybind11 --includes) -I$(MDTROOT)/cpp/include
LDFLAGS += -shared -L$(MDTROOT) -lMDT

.PHONY: clean Execs

%o::%cc
	$(CXX) $(CXXFLAGS) -c $? -o $@

TARGET=MDT$(shell python3-config --extension-suffix)
all: PyWrapMDT.o
	$(RM) $@
	$(LD) $^ $(LDFLAGS) -o $(TARGET)
	@echo "$@ done"

clean:
	$(RM) *.o *.so
