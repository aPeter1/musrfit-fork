#---------------------------------------------------
# Makefile
#
# Author: Andreas Suter
# e-mail: andreas.suter@psi.ch
#
# $Id$
#
#---------------------------------------------------

#---------------------------------------------------
# get compilation and library flags from root-config

ROOTCFLAGS    = $(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --libs)
ROOTGLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs)

#---------------------------------------------------
# depending on the architecture, choose the compiler,
# linker, and the flags to use
#

OSTYPE = $(shell uname)

ifeq ($(OSTYPE),Linux)
OS = LINUX
endif
ifeq ($(OSTYPE),Linux-gnu)
OS = LINUX
endif
ifeq ($(OSTYPE),darwin)
OS = DARWIN
endif

# -- Linux
ifeq ($(OS),LINUX)
CXX           = g++
CXXFLAGS      = -Wall -Wno-trigraphs -fPIC
INCLUDES      = -I../include
LD            = g++
LDFLAGS       = -g
SOFLAGS       = -O -shared
endif

# -- Darwin
ifeq ($(OS),DARWIN)
CXX           = g++
CXXFLAGS      = -Wall -Wno-trigraphs -fPIC
INCLUDES      = -I../include
LD            = g++
LDFLAGS       = -g
SOFLAGS       = -dynamic
endif

# the output from the root-config script:
CXXFLAGS      += $(ROOTCFLAGS)
LDFLAGS       +=

# the ROOT libraries (G = graphic)
LIBS          = $(ROOTLIBS) -lXMLParser
GLIBS         = $(ROOTGLIBS) -lXMLParser

# some definitions: headers (used to generate *Dict* stuff), sources, objects,...
OBJS =
OBJS += PNL_StartupHandler.o PNL_StartupHandlerDict.o
OBJS += PNL_RgeHandler.o
OBJS += PNL_PippardFitter.o PNL_PippardFitterDict.o

SHLIB = libPNL_PippardFitter.so

# make the shared lib:
#
all:  $(SHLIB)

$(SHLIB): $(OBJS)
	@echo "---> Building shared library $(SHLIB) ..."
	/bin/rm -f $(SHLIB)
	$(LD) $(OBJS) $(SOFLAGS) -o $(SHLIB) $(LIBS)
	@echo "done"

# clean up: remove all object file (and core files)
# semicolon needed to tell make there is no source
# for this target!
#
clean:; 	@rm -f $(OBJS) *Dict* core*
	@echo "---> removing $(OBJS)"

#
$(OBJS): %.o: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $<

PNL_StartupHandlerDict.cpp: PNL_StartupHandler.h PNL_StartupHandlerLinkDef.h
	@echo "Generating dictionary $@..."
	rootcint -f $@ -c -p $^

PNL_PippardFitterDict.cpp: PNL_PippardFitter.h PNL_PippardFitterLinkDef.h
	@echo "Generating dictionary $@..."
	rootcint -f $@ -c -p $^
	
install: all
	@echo "Installing shared lib: libPUserFcn.so ( you must be root ;-) )"
ifeq ($(OS),LINUX)
	cp -pv $(SHLIB) $(ROOTSYS)/lib
#	cp -pv ../include/PNonlocal.h $(ROOTSYS)/include
#	cp -pv ../include/PNL_StartupHandler.h $(ROOTSYS)/include
#	cp -pv ../include/PNL_PippardFitter.h $(ROOTSYS)/include
endif
