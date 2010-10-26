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
ifeq ($(OSTYPE),Darwin)
OS = DARWIN
endif

# -- Linux
ifeq ($(OS),LINUX)
CXX           = g++
CXXFLAGS      = -g -Wall -Wno-trigraphs -fPIC
INCLUDES      = -I./
LD            = g++
LDFLAGS       = -g
SOFLAGS       = -O -shared
endif

# -- Darwin
ifeq ($(OS),DARWIN)
CXX           = g++
CXXFLAGS      = -g -Wall -Wno-trigraphs -fPIC
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

EXEC = eventHandlerTest

# some definitions: headers (used to generate *Dict* stuff), sources, objects,...
OBJS =
OBJS += $(EXEC).o
OBJS += PEventHandlerTest.o PEventHandlerTestDict.o

# make the executable:
#
all:  $(EXEC)

$(EXEC): $(OBJS)
	@echo "---> Building $(EXEC) ..."
	$(LD) $(OBJS) -o $(EXEC) $(GLIBS)
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

PEventHandlerTestDict.cpp: ./PEventHandlerTest.h ./PEventHandlerTestLinkDef.h
	@echo "Generating dictionary $@..."
	rootcint -v -f $@ -c -p $^
