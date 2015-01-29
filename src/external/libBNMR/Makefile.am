## Process this file with automake to create Makefile.in

h_sources = \
  TBNMR.h 

h_linkdef = \
  TBNMRLinkDef.h

dict_h_sources = \
  TBNMRDict.h

cpp_sources = \
  TBNMR.cpp

dict_cpp_sources = \
  TBNMRDict.cpp

include_HEADERS = $(h_sources)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources)

AM_CPPFLAGS = -I$(top_srcdir)/src/include $(LEM_CFLAGS) $(PMUSR_CFLAGS) $(CUBA_CFLAGS) -I$(ROOTINCDIR)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *~ core

%Dict.cpp %Dict.h: %.h %LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cpp -c -p $(INCLUDES) $^

lib_LTLIBRARIES = libBNMR.la

libBNMR_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libBNMR_la_LIBADD = $(CUBA_LIBS) $(ROOT_LIBS)
libBNMR_la_LDFLAGS = -version-info $(PLUGIN_LIBRARY_VERSION) -release $(PLUGIN_RELEASE) $(AM_LDFLAGS)

## However, create some symbolic links to the shared library
## in order to unify the function call on different operating systems

if IS_DARWIN
install-exec-hook:
	$(LN_S) -f $(libdir)/libBNMR.dylib $(libdir)/libBNMR.so
uninstall-hook:
	rm -f $(libdir)/libBNMR.so
endif

if IS_CYGWIN
install-exec-hook:
	$(LN_S) -f $(bindir)/cygBNMR-$(PLUGIN_MAJOR_VERSION)-$(PLUGIN_MINOR_VERSION)-$(PLUGIN_MAJOR_VERSION).dll $(libdir)/libBNMR.so
uninstall-hook:
	rm -f $(libdir)/libBNMR.so
endif
