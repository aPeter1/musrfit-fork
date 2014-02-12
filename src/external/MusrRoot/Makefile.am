## Process this file with automake to create Makefile.in

h_sources = \
  TMusrRunHeader.h

h_linkdef = \
  TMusrRunHeaderLinkDef.h

dict_h_sources = \
  TMusrRunHeaderDict.h

cpp_sources = \
  TMusrRunHeader.cpp

dict_cpp_sources = \
  TMusrRunHeaderDict.cpp 

include_HEADERS = $(h_sources)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources)

AM_CPPFLAGS = -I. -I$(ROOTINCDIR)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dist_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *~ core

%Dict.cpp %Dict.h: %.h %LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cpp -c -p $(INCLUDES) $^

lib_LTLIBRARIES = libTMusrRunHeader.la

libTMusrRunHeader_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libTMusrRunHeader_la_LIBADD = $(ROOT_LIBS)
libTMusrRunHeader_la_LDFLAGS = -version-info $(MUSR_ROOT_LIBRARY_VERSION) -release $(MUSR_ROOT_RELEASE) $(AM_LDFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = TMusrRunHeader.pc

