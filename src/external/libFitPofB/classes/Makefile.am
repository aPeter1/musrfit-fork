## Process this file with automake to create Makefile.in
## $Id$

h_sources = \
  ../include/TBofZCalc.h \
  ../include/TBulkTriVortexFieldCalc.h \
  ../include/TLondon1D.h \
  ../include/TPofBCalc.h \
  ../include/TPofTCalc.h \
  ../include/TSkewedGss.h \
  ../include/TVortex.h

h_linkdef = \
  ../include/TLondon1DLinkDef.h \
  ../include/TVortexLinkDef.h \
  ../include/TSkewedGssLinkDef.h

dict_h_sources = \
  TLondon1DDict.h \
  TSkewedGssDict.h \
  TVortexDict.h

cpp_sources = \
  TBulkTriVortexFieldCalc.cpp \
  TBofZCalc.cpp \
  TLondon1D.cpp \
  TPofBCalc.cpp \
  TPofTCalc.cpp \
  TSkewedGss.cpp \
  TVortex.cpp

dict_cpp_sources = \
  TLondon1DDict.cpp \
  TSkewedGssDict.cpp \
  TVortexDict.cpp

include_HEADERS = $(h_sources)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources)

AM_CPPFLAGS = -I$(top_srcdir)/src/include -I../include $(BMWTOOLS_CFLAGS) $(LEM_CFLAGS) $(PMUSR_CFLAGS) $(FFTW3_CFLAGS) -I$(ROOTINCDIR)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *~ ../include/*~ core

%Dict.cpp %Dict.h: ../include/%.h ../include/%LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cpp -c -p $(INCLUDES) $^

lib_LTLIBRARIES = libFitPofB.la

libFitPofB_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libFitPofB_la_LIBADD = $(BMWTOOLS_LIBS) $(LEM_LIBS) $(USERFCN_LIBS) $(FFTW3_LIBS) $(ROOT_LIBS)
libFitPofB_la_LDFLAGS = -version-info $(PLUGIN_LIBRARY_VERSION) -release $(PLUGIN_RELEASE) $(AM_LDFLAGS)

## For the moment do not build pkgconfig files for musrfit plug-ins...
## pkgconfigdir = $(libdir)/pkgconfig
## pkgconfig_DATA = PTFitPofB.pc

## However, create some symbolic links to the shared library
## in order to unify the function call on different operating systems

if IS_DARWIN
install-exec-hook:
	$(LN_S) -f $(libdir)/libFitPofB.dylib $(libdir)/libFitPofB.so
uninstall-hook:
	rm -f $(libdir)/libFitPofB.so
endif

if IS_CYGWIN
install-exec-hook:
	$(LN_S) -f $(bindir)/cygFitPofB-$(PLUGIN_MAJOR_VERSION)-$(PLUGIN_MINOR_VERSION)-$(PLUGIN_MAJOR_VERSION).dll $(libdir)/libFitPofB.so
uninstall-hook:
	rm -f $(libdir)/libFitPofB.so
endif
