#---------------------------------------------------
# Makefile
#
# Author: Andreas Suter
# e-mail: andreas.suter@psi.ch
#
# $Id: Makefile 5052 2012-01-12 09:55:19Z suter_a $
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
PSILIBS       = -lTMusrRunHeader

EXEC  = 
EXEC += musrRoot_runHeader_test
EXEC += write_musrRoot_runHeader
EXEC += read_musrRoot_runHeader

# some definitions: headers (used to generate *Dict* stuff), sources, objects,...
OBJS =
OBJS += musrRoot_runHeader_test.o
OBJS += write_musrRoot_runHeader.o
OBJS += read_musrRoot_runHeader.o

# make the executable:
#
all:  $(EXEC)

musrRoot_runHeader_test: musrRoot_runHeader_test.o
	@echo "---> Building musrRoot_runHeader_test ..."
	$(LD) musrRoot_runHeader_test.o -o musrRoot_runHeader_test $(GLIBS) $(PSILIBS)  
	@echo "done"

write_musrRoot_runHeader: write_musrRoot_runHeader.o
	@echo "---> Building write_musrRoot_runHeader ..."
	$(LD) write_musrRoot_runHeader.o -o write_musrRoot_runHeader $(GLIBS) $(PSILIBS)  
	@echo "done"

read_musrRoot_runHeader: read_musrRoot_runHeader.o
	@echo "---> Building read_musrRoot_runHeader ..."
	$(LD) read_musrRoot_runHeader.o -o read_musrRoot_runHeader $(GLIBS) $(PSILIBS)  
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
