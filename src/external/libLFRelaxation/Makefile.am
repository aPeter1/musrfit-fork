## Process this file with automake to create Makefile.in

h_sources = \
  TLFRelaxation.h \
  ../BMWIntegrator/BMWIntegrator.h

h_linkdef = \
  TLFRelaxationLinkDef.h

dict_h_sources = \
  TLFRelaxationDict.h

cpp_sources = \
  TLFRelaxation.cpp \
  ../BMWIntegrator/BMWIntegrator.cpp

dict_cpp_sources = \
  TLFRelaxationDict.cpp

include_HEADERS = $(h_sources)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources)

INCLUDES = -I$(top_srcdir)/src/include $(PMUSR_CFLAGS) $(FFTW3_CFLAGS) $(GSL_CFLAGS) $(ROOT_CFLAGS) $(CUBA_CFLAGS)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *~ core

%Dict.cpp %Dict.h: %.h %LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cpp -c -p $(INCLUDES) $^

lib_LTLIBRARIES = libLFRelaxation.la

libLFRelaxation_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libLFRelaxation_la_LIBADD = $(PMUSR_LIBS) $(FFTW3_THREADS_LIBS) $(GSL_LIBS) $(ROOT_LIBS) $(CUBA_LIBS)
libLFRelaxation_la_LDFLAGS = -version-info $(PLUGIN_LIBRARY_VERSION) -release $(PLUGIN_RELEASE) $(AM_LDFLAGS)

## For the moment do not build pkgconfig files for musrfit plug-ins...
## pkgconfigdir = $(libdir)/pkgconfig
## pkgconfig_DATA = LFRelaxation.pc

