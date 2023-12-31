#---------------------------------------------------
# Makefile
#
# Author: Andreas Suter
# e-mail: andreas.suter@psi.ch
#
# $Id$
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
CXXFLAGS      = -g -Wall -fPIC
INCLUDES      = -I ./
LD            = g++
LDFLAGS       = -g
endif

# -- Darwin
ifeq ($(OS),DARWIN)
CXX           = g++
CXXFLAGS      = -g -Wall -fPIC
INCLUDES      = -I../include
LD            = g++
LDFLAGS       = -g
endif

# the output from the root-config script:
CXXFLAGS      += $(ROOTCFLAGS)
LDFLAGS       +=

# the ROOT libraries (G = graphic)
LIBS          = $(ROOTLIBS) -lXMLParser
GLIBS         = $(ROOTGLIBS) -lXMLParser

# UserFcn lib
USERFCNLIB    = -lTUserFcnBase 
#-lTUserFcn

INSTALLPATH   = ./

EXEC = userFcnTest

# some definitions: headers, sources, objects,...
OBJS =
OBJS += $(EXEC).o

# make the executable:
#
all:  $(EXEC)

$(EXEC): $(OBJS)
	@echo "---> Building $(EXEC) ..."
	/bin/rm -f $(SHLIB)
	$(LD) $(OBJS) -o $(EXEC) $(LIBS) $(USERFCNLIB)
	@echo "done"

# clean up: remove all object file (and core files)
# semicolon needed to tell make there is no source
# for this target!
#
clean:; 	@rm -f $(OBJS)
	@echo "---> removing $(OBJS)"

#
$(OBJS): %.o: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $<

install: all
	@echo "copy $(EXEC) to $(INSTALLPATH)"
	cp -p $(EXEC) $(INSTALLPATH)

