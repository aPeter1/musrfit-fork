#---------------------------------------------------
# Makefile
#
# Author: Andreas Suter
# e-mail: andreas.suter@psi.ch
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
GSLPATH       = /usr/include/gsl
EIGEN2PATH    = /usr/local/include/eigen2
INCLUDES      = -I$(PMUSRPATH) -I$(MNPATH) -I$(GSLPATH) -I$(EIGEN2PATH)
LD            = g++
LDFLAGS       = -O
INSTALLPATH   = $(ROOTSYS)/bin
EXEC          = nonlocal
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
# GSL lib
GSLLIB        = -L/usr/lib -lgsl

# some definitions: headers, sources, objects,...
OBJS =
OBJS += nonlocal.o PPippard.o

# make the executable:
#
all:  $(EXEC)

nonlocal$(SUFFIX): $(OBJS)
	@echo "---> Building $@ ..."
	/bin/rm -f $@
	$(LD) $< -o $@ PPippard.o $(LDFLAGS) $(GLIBS) $(PSILIBS) $(MNLIB) $(MMLIB) $(GSLLIB)
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
	cp -fvp external/scripts/msr2data $(INSTALLPATH)
	chmod 755 $(INSTALLPATH)/msr2data
