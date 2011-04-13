## Process this file with automake to create Makefile.in
## $Id$

h_sources = \
  TCalcMeanFieldsLEM.h

h_linkdef = \
  TCalcMeanFieldsLEMLinkDef.h

dict_h_sources = \
  TCalcMeanFieldsLEMDict.h

cpp_sources = \
  TCalcMeanFieldsLEM.cpp

dict_cpp_sources = \
  TCalcMeanFieldsLEMDict.cpp

include_HEADERS = $(h_sources)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources)

INCLUDES = -I$(top_srcdir)/src/include $(BMWTOOLS_CFLAGS) $(FITPOFB_CFLAGS) $(PMUSR_CFLAGS) $(ROOT_CFLAGS) $(FFTW3_CFLAGS)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *~ core

%Dict.cpp %Dict.h: %.h %LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cpp -c -p $(INCLUDES) $^

lib_LTLIBRARIES = libCalcMeanFieldsLEM.la

libCalcMeanFieldsLEM_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libCalcMeanFieldsLEM_la_LIBADD = $(BMWTOOLS_LIBS) $(FITPOFB_LIBS) $(ROOT_LIBS)
libCalcMeanFieldsLEM_la_LDFLAGS = -version-info $(PLUGIN_LIBRARY_VERSION) -release $(PLUGIN_RELEASE) $(AM_LDFLAGS)

## For the moment do not build pkgconfig files for musrfit plug-ins...
## pkgconfigdir = $(libdir)/pkgconfig
## pkgconfig_DATA = CalcMeanFieldsLEM.pc

## However, create some symbolic links to the shared library
## in order to unify the function call on different operating systems

if IS_DARWIN
install-exec-hook:
	$(LN_S) $(libdir)/libCalcMeanFieldsLEM.dylib $(libdir)/libCalcMeanFieldsLEM.so
uninstall-hook:
	rm -f $(libdir)/libCalcMeanFieldsLEM.so
endif

if IS_CYGWIN
install-exec-hook:
	$(LN_S) $(bindir)/cygCalcMeanFieldsLEM-$(PLUGIN_MAJOR_VERSION)-$(PLUGIN_MINOR_VERSION)-$(PLUGIN_MAJOR_VERSION).dll $(libdir)/libCalcMeanFieldsLEM.so
uninstall-hook:
	rm -f $(libdir)/libCalcMeanFieldsLEM.so
endif
