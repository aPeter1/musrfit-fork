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

# PSI libs
PSILIBS       = -lTPsiRunHeader

EXEC  = 
EXEC += psi_runHeader_test
EXEC += write_psi_runHeader
EXEC += read_psi_runHeader

# some definitions: headers (used to generate *Dict* stuff), sources, objects,...
OBJS =
OBJS += psi_runHeader_test.o
OBJS += write_psi_runHeader.o
OBJS += read_psi_runHeader.o

# make the executable:
#
all:  $(EXEC)

psi_runHeader_test: psi_runHeader_test.o
	@echo "---> Building psi_runHeader_test ..."
	$(LD) psi_runHeader_test.o -o psi_runHeader_test $(GLIBS) $(PSILIBS)  
	@echo "done"

write_psi_runHeader: write_psi_runHeader.o
	@echo "---> Building write_psi_runHeader ..."
	$(LD) write_psi_runHeader.o -o write_psi_runHeader $(GLIBS) $(PSILIBS)  
	@echo "done"

read_psi_runHeader: read_psi_runHeader.o
	@echo "---> Building read_psi_runHeader ..."
	$(LD) read_psi_runHeader.o -o read_psi_runHeader $(GLIBS) $(PSILIBS)  
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
