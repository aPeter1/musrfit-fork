## Process this file with automake to create Makefile.in

h_sources = \
  BMWStartupHandler.h \
  TTrimSPDataHandler.h \
  BMWIntegrator.h

h_linkdef = \
  BMWStartupHandlerLinkDef.h

dict_h_sources = \
  BMWStartupHandlerDict.h

cpp_sources = \
  BMWStartupHandler.cpp \
  TTrimSPDataHandler.cpp \
  BMWIntegrator.cpp

dict_cpp_sources = \
  BMWStartupHandlerDict.cpp

pcmdir = $(libdir)
pcm_DATA = BMWStartupHandlerDict_rdict.pcm

include_HEADERS = $(h_sources)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources)

AM_CPPFLAGS = -I$(top_srcdir)/src/include $(LEM_CFLAGS) $(PMUSR_CFLAGS) $(CUBA_CFLAGS) -I$(ROOTINCDIR)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *Dict* *~ core

%Dict.cpp %Dict.h: %.h %LinkDef.h
	@ROOTCLING@ -v -f $*Dict.cpp -c -p $(INCLUDES) $^

lib_LTLIBRARIES = libBMWtools.la

libBMWtools_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libBMWtools_la_LIBADD = $(CUBA_LIBS) $(ROOT_LIBS)
libBMWtools_la_LDFLAGS = -version-info $(PLUGIN_LIBRARY_VERSION) -release $(PLUGIN_RELEASE) $(AM_LDFLAGS)

## For the moment do not build pkgconfig files for musrfit plug-ins...
## pkgconfigdir = $(libdir)/pkgconfig
## pkgconfig_DATA = PTFitPofB.pc

## However, create some symbolic links to the shared library
## in order to unify the function call on different operating systems

if IS_DARWIN
install-exec-hook:
	$(LN_S) -f $(libdir)/libBMWtools.dylib $(libdir)/libBMWtools.so
uninstall-hook:
	rm -f $(libdir)/libBMWtools.so
endif

if IS_CYGWIN
install-exec-hook:
	$(LN_S) -f $(bindir)/cygBMWtools-$(PLUGIN_MAJOR_VERSION)-$(PLUGIN_MINOR_VERSION)-$(PLUGIN_MAJOR_VERSION).dll $(libdir)/libBMWtools.so
uninstall-hook:
	rm -f $(libdir)/libBMWtools.so
endif
