## Process this file with automake to create Makefile.in

h_sources = \
  TLemStats.h \
  TLemRunHeader.h

h_linkdef = \
  TLemStatsLinkDef.h \
  TLemRunHeaderLinkDef.h

dict_h_sources = \
  TLemStatsDict.h \
  TLemRunHeaderDict.h

cpp_sources = \
  TLemStats.cxx \
  TLemRunHeader.cxx

dict_cpp_sources = \
  TLemStatsDict.cxx \
  TLemRunHeaderDict.cxx

pcmdir = $(libdir)
pcm_DATA = \
  TLemStatsDict_rdict.pcm \
  TLemRunHeaderDict_rdict.pcm

include_HEADERS = $(h_sources)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources)

AM_CPPFLAGS = -I. -I$(ROOTINCDIR)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dist_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cxx *Dict.h *Dict* *~ core

%Dict.cxx %Dict.h: %.h %LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cxx -c -p $(INCLUDES) $^

lib_LTLIBRARIES = libTLemRunHeader.la

libTLemRunHeader_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libTLemRunHeader_la_LIBADD = $(ROOT_LIBS)
libTLemRunHeader_la_LDFLAGS = -version-info $(LEM_LIBRARY_VERSION) -release $(LEM_RELEASE) $(AM_LDFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = TLemRunHeader.pc

