## Process this file with automake to create Makefile.in

h_sources = \
  ZFRelaxation.h

h_linkdef = \
  ZFRelaxationLinkDef.h

dict_h_sources = \
  ZFRelaxationDict.h

cpp_sources = \
  ZFRelaxation.cpp

dict_cpp_sources = \
  ZFRelaxationDict.cpp

pcmdir = $(libdir)
pcm_DATA = ZFRelaxationDict_rdict.pcm

include_HEADERS = $(h_sources)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources)

AM_CPPFLAGS = -I$(top_srcdir)/src/include $(BMWTOOLS_CFLAGS) $(PMUSR_CFLAGS) -I$(ROOTINCDIR)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *Dict* *~ core

%Dict.cpp %Dict.h: %.h %LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cpp -c -p $(AM_CPPFLAGS) $^

lib_LTLIBRARIES = libZFRelaxation.la

libZFRelaxation_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libZFRelaxation_la_LIBADD = $(BMWTOOLS_LIBS) $(USERFCN_LIBS) $(GSL_LIBS) $(ROOT_LIBS)
libZFRelaxation_la_LDFLAGS = -version-info $(PLUGIN_LIBRARY_VERSION) -release $(PLUGIN_RELEASE) $(AM_LDFLAGS)

## For the moment do not build pkgconfig files for musrfit plug-ins...
## pkgconfigdir = $(libdir)/pkgconfig
## pkgconfig_DATA = ZFRelaxation.pc

## However, create some symbolic links to the shared library
## in order to unify the function call on different operating systems

if IS_DARWIN
install-exec-hook:
	$(LN_S) -f $(libdir)/libZFRelaxation.dylib $(libdir)/libZFRelaxation.so
uninstall-hook:
	rm -f $(libdir)/libZFRelaxation.so
endif

if IS_CYGWIN
install-exec-hook:
	$(LN_S) -f $(bindir)/cygZFRelaxation-$(PLUGIN_MAJOR_VERSION)-$(PLUGIN_MINOR_VERSION)-$(PLUGIN_MAJOR_VERSION).dll $(libdir)/libZFRelaxation.so
uninstall-hook:
	rm -f $(libdir)/libZFRelaxation.so
endif
