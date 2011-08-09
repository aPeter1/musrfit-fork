## Process this file with automake to create Makefile.in

h_sources = \
  PMagProximityFitter.h \
  PMPRgeHandler.h \
  PMPStartupHandler.h \
  PMagProximity.h

h_linkdef = \
  PMagProximityFitterLinkDef.h \
  PMPStartupHandlerLinkDef.h

dict_h_sources = \
  PMagProximityFitterDict.h \
  PMPStartupHandlerDict.h

cpp_sources = \
  PMagProximityFitter.cpp \
  PMPRgeHandler.cpp \
  PMPStartupHandler.cpp

dict_cpp_sources = \
  PMagProximityFitterDict.cpp \
  PMPStartupHandlerDict.cpp

include_HEADERS = $(h_sources)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources)

INCLUDES = -I$(top_srcdir)/src/include -I../include $(PMUSR_CFLAGS) $(FFTW3_CFLAGS) $(ROOT_CFLAGS)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *~ core

%Dict.cpp %Dict.h: %.h %LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cpp -c -p $(INCLUDES) $^

lib_LTLIBRARIES = libPMagProximityFitter.la

libPMagProximityFitter_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libPMagProximityFitter_la_LIBADD = $(USERFCN_LIBS) $(FFTW3_LIBS) $(ROOT_LIBS)
libPMagProximityFitter_la_LDFLAGS = -version-info $(PLUGIN_LIBRARY_VERSION) -release $(PLUGIN_RELEASE) $(AM_LDFLAGS)

## For the moment do not build pkgconfig files for musrfit plug-ins...
## pkgconfigdir = $(libdir)/pkgconfig
## pkgconfig_DATA = PMagProximityFitter.pc

## However, create some symbolic links to the shared library
## in order to unify the function call on different operating systems

if IS_DARWIN
install-exec-hook:
	$(LN_S) -f $(libdir)/libPMagProximityFitter.dylib $(libdir)/libPMagProximityFitter.so
uninstall-hook:
	rm -f $(libdir)/libPMagProximityFitter.so
endif

if IS_CYGWIN
install-exec-hook:
	$(LN_S) -f $(bindir)/cygPMagProximityFitter-$(PLUGIN_MAJOR_VERSION)-$(PLUGIN_MINOR_VERSION)-$(PLUGIN_MAJOR_VERSION).dll $(libdir)/libPMagProximityFitter.so
uninstall-hook:
	rm -f $(libdir)/libPMagProximityFitter.so
endif
