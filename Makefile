CC=g++
CXX=g++

CXXVERSION=-std=c++20

CODESRC= VirtualMemory.cpp
OBJ= VirtualMemory.o
EXESRC= $(CODESRC)
EXEOBJ= libVirtualMemory.a

INCS=-I.
CFLAGS = $(CXXVERSION) -O3 -Wall $(INCS)
CXXFLAGS = $(CXXVERSION) -O3 -Wall $(INCS)

TARGETS = $(EXEOBJ)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex4.tar
TARSRCS=$(CODESRC) Makefile README

all: $(TARGETS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
$(EXEOBJ): $(EXESRC:.cpp=.o)
	ar rcs $@ $^
	$(RM) $^

clean:
	$(RM) $(TARGETS) $(EXESRC:.cpp=.o)

depend:
	makedepend -- $(CFLAGS) -- $(CODESRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
