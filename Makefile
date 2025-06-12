CC=g++
CXX=g++
RANLIB=ranlib
AR=ar
ARFLAGS=rcs

LIBSRC=Resources/VirtualMemory.cpp Resources/PhysicalMemory.cpp
LIBOBJ=$(LIBSRC:.cpp=.o)

INCS=-IResources
CFLAGS=-Wall -std=c++11 -g $(INCS)
CXXFLAGS=$(CFLAGS)

VMLIB=libVirtualMemory.a
TARGETS=$(VMLIB)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex4.tar
TARSRCS=$(LIBSRC) Makefile README.md

all: $(TARGETS)

$(TARGETS): $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

clean:
	$(RM) $(TARGETS) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
