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

ARCH = $(shell $(ROOTSYS)/bin/root-config --arch)

ifeq ($(ARCH),linux)
OS = LINUX
endif
ifeq ($(ARCH),linuxx8664gcc)
OS = LINUX
endif
ifeq ($(ARCH),win32gcc)
OS = WIN32GCC
endif
ifeq ($(ARCH),macosx)
OS = DARWIN
endif

# -- Linux
ifeq ($(OS),LINUX)
CXX           = g++
CXXFLAGS      = -O3 -Wall -fPIC
PMUSRPATH     = ./include
MNPATH        = $(ROOTSYS)/include
INCLUDES      = -I$(PMUSRPATH) -I$(MNPATH)
LD            = g++
LDFLAGS       = -O
INSTALLPATH   = $(ROOTSYS)/bin
EXEC          = musrfit musrview musrparam musrt0 msr2msr
SUFFIX        =
endif

# -- Windows/Cygwin
ifeq ($(OS),WIN32GCC)
CXX           = g++
CXXFLAGS      = -O3 -Wall
PMUSRPATH     = ./include
MNPATH        = $(ROOTSYS)/include
BOOSTPATH     = /usr/include
INCLUDES      = -I$(PMUSRPATH) -I$(MNPATH) -I$(BOOSTPATH)
LD            = g++
LDFLAGS       = -O -Wl,--enable-auto-import -Wl,--enable-runtime-pseudo-reloc
INSTALLPATH   = $(ROOTSYS)/bin
EXEC          = musrfit.exe musrview.exe musrparam.exe musrt0.exe msr2msr.exe
SUFFIX        = .exe
endif

# -- MacOSX/Darwin
ifeq ($(OS),DARWIN)
CXX           = g++
CXXFLAGS      = -O3 -Wall -fPIC
PMUSRPATH     = ./include
MNPATH        = $(ROOTSYS)/include
BOOSTPATH     = /sw/include
INCLUDES      = -I$(PMUSRPATH) -I$(MNPATH) -I$(BOOSTPATH)
LD            = g++
LDFLAGS       = -O
INSTALLPATH   = $(ROOTSYS)/bin
EXEC          = musrfit musrview musrparam musrt0 msr2msr
SUFFIX        =
endif

# the output from the root-config script:
CXXFLAGS      += $(ROOTCFLAGS)
LDFLAGS       +=

# the ROOT libraries (G = graphic)
LIBS          = $(ROOTLIBS) -lXMLParser
GLIBS         = $(ROOTGLIBS) -lXMLParser

# PSI libs
PSILIBS       = -L$(ROOTSYS)/lib -lTLemRunHeader -lPMusr
# Minuit2 lib
MNLIB         = -L$(ROOTSYS)/lib -lMinuit2
# MathMore lib
MMLIB         = -L$(ROOTSYS)/lib -lMathMore

# some definitions: headers, sources, objects,...
OBJS =
OBJS += musrfit.o musrview.o musrparam.o musrt0.o msr2msr.o

# make the executable:
#
all:  $(EXEC)

musrfit$(SUFFIX): musrfit.o
	@echo "---> Building $@ ..."
	/bin/rm -f $@
	$(LD) $< -o $@ $(LDFLAGS) $(GLIBS) $(PSILIBS) $(MNLIB) $(MMLIB)
	@echo "done"

musrview$(SUFFIX): musrview.o
	@echo "---> Building $@ ..."
	/bin/rm -f $@
	$(LD) $< -o $@ $(LDFLAGS) $(GLIBS) $(PSILIBS) $(MNLIB) $(MMLIB)
	@echo "done"

musrt0$(SUFFIX): musrt0.o
	@echo "---> Building $@ ..."
	/bin/rm -f $@
	$(LD) $< -o $@ $(LDFLAGS) $(GLIBS) $(PSILIBS) $(MNLIB) $(MMLIB)
	@echo "done"

musrparam$(SUFFIX): musrparam.o
	@echo "---> Building $@ ..."
	/bin/rm -f $@
	$(LD) $< -o $@ $(LDFLAGS) $(GLIBS) $(PSILIBS) $(MNLIB) $(MMLIB)
	@echo "done"

msr2msr$(SUFFIX): msr2msr.o
	@echo "---> Building $@ ..."
	/bin/rm -f $@
	$(LD) $< -o $@ $(LDFLAGS) $(GLIBS) $(PSILIBS) $(MNLIB) $(MMLIB)
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
	cp -fvp $(EXEC) $(INSTALLPATH)
	cp -fvp musrfit_startup.xml $(INSTALLPATH)
