## Process this file with automake to create Makefile.in

h_sources = \
  PNL_PippardFitter.h \
  PNL_RgeHandler.h \
  PNL_StartupHandler.h \
  PNonlocal.h

h_linkdef = \
  PNL_PippardFitterLinkDef.h \
  PNL_StartupHandlerLinkDef.h

dict_h_sources = \
  PNL_PippardFitterDict.h \
  PNL_StartupHandlerDict.h

cpp_sources = \
  PNL_PippardFitter.cpp \
  PNL_RgeHandler.cpp \
  PNL_StartupHandler.cpp

dict_cpp_sources = \
  PNL_PippardFitterDict.cpp \
  PNL_StartupHandlerDict.cpp

include_HEADERS = $(h_sources)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources)

INCLUDES = -I$(top_srcdir)/src/include -I../include $(PMUSR_CFLAGS) $(FFTW3_CFLAGS) $(ROOT_CFLAGS)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *~ core

%Dict.cpp %Dict.h: %.h %LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cpp -c -p $(INCLUDES) $^

lib_LTLIBRARIES = libPNL_PippardFitter.la

libPNL_PippardFitter_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libPNL_PippardFitter_la_LIBADD = $(PMUSR_LIBS) $(FFTW3_LIBS) $(ROOT_LIBS)
libPNL_PippardFitter_la_LDFLAGS = -version-info $(PLUGIN_LIBRARY_VERSION) -release $(PLUGIN_RELEASE) $(AM_LDFLAGS)

## For the moment do not build pkgconfig files for musrfit plug-ins...
## pkgconfigdir = $(libdir)/pkgconfig
## pkgconfig_DATA = PNL_PippardFitter.pc

## However, create some symbolic links to the shared library
## in order to unify the function call on different operating systems

if IS_DARWIN
install-exec-hook:
	$(LN_S) $(libdir)/libPNL_PippardFitter.dylib $(libdir)/libPNL_PippardFitter.so
uninstall-hook:
	rm -f $(libdir)/libPNL_PippardFitter.so
endif

if IS_CYGWIN
install-exec-hook:
	$(LN_S) $(bindir)/cygPNL_PippardFitter-$(PLUGIN_MAJOR_VERSION)-$(PLUGIN_MINOR_VERSION)-$(PLUGIN_MAJOR_VERSION).dll $(libdir)/libPNL_PippardFitter.so
uninstall-hook:
	rm -f $(libdir)/libPNL_PippardFitter.so
endif
