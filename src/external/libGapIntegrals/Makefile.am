## Process this file with automake to create Makefile.in

h_sources = \
  TGapIntegrals.h

h_linkdef = \
  TGapIntegralsLinkDef.h

dict_h_sources = \
  TGapIntegralsDict.h

cpp_sources = \
  TGapIntegrals.cpp

dict_cpp_sources = \
  TGapIntegralsDict.cpp

include_HEADERS = $(h_sources)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources)

INCLUDES = -I$(top_srcdir)/src/include $(BMWTOOLS_CFLAGS) $(PMUSR_CFLAGS) $(GSL_CFLAGS) $(ROOT_CFLAGS) $(CUBA_CFLAGS)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *~ core

%Dict.cpp %Dict.h: %.h %LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cpp -c -p $(INCLUDES) $^

lib_LTLIBRARIES = libGapIntegrals.la

libGapIntegrals_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libGapIntegrals_la_LIBADD = $(BMWTOOLS_LIBS) $(PMUSR_LIBS) $(GSL_LIBS) $(ROOT_LIBS) $(CUBA_LIBS)
libGapIntegrals_la_LDFLAGS = -version-info $(PLUGIN_LIBRARY_VERSION) -release $(PLUGIN_RELEASE) $(AM_LDFLAGS)

## For the moment do not build pkgconfig files for musrfit plug-ins...
## pkgconfigdir = $(libdir)/pkgconfig
## pkgconfig_DATA = GapIntegrals.pc

## However, create some symbolic links to the shared library
## in order to unify the function call on different operating systems

if IS_DARWIN
install-exec-hook:
	$(LN_S) $(libdir)/libGapIntegrals.dylib $(libdir)/libGapIntegrals.so
uninstall-hook:
	rm -f $(libdir)/libGapIntegrals.so
endif

if IS_CYGWIN
install-exec-hook:
	$(LN_S) $(bindir)/cygGapIntegrals-$(PLUGIN_MAJOR_VERSION)-$(PLUGIN_MINOR_VERSION)-$(PLUGIN_MAJOR_VERSION).dll $(libdir)/libGapIntegrals.so
uninstall-hook:
	rm -f $(libdir)/libGapIntegrals.so
endif
