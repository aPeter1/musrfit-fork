## Process this file with automake to create Makefile.in

h_sources = \
  ../include/PStartupHandler_PM.h \
  ../include/PPhotoMeissner.h

h_linkdef = \
  ../include/PStartupHandler_PMLinkDef.h \
  ../include/PPhotoMeissnerLinkDef.h

dict_h_sources = \
  ../include/PStartupHandler_PMDict.h \
  ../include/PPhotoMeissnerDict.h

cpp_sources = \
  PStartupHandler_PM.cpp \
  PPhotoMeissner.cpp

dict_cpp_sources = \
  PStartupHandler_PMDict.cpp \
  PPhotoMeissnerDict.cpp

pcmdir = $(libdir)
pcm_DATA = \
  PPhotoMeissnerDict_rdict.pcm \
  PStartupHandler_PMDict_rdict.pcm

include_HEADERS = $(h_sources)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources)

AM_CPPFLAGS = -I$(top_srcdir)/src/include -I../include $(PMUSR_CFLAGS) $(FFTW3_CFLAGS) -I$(ROOTINCDIR) $(GSL_CFLAGS)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = ../classes/*Dict* ../include/*Dict.* *~ core

%Dict.cpp %Dict.h: ../include/%.h ../include/%LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cpp -c -p $(AM_CPPFLAGS) $^

lib_LTLIBRARIES = libPPhotoMeissner.la

libPPhotoMeissner_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libPPhotoMeissner_la_LIBADD = $(USERFCN_LIBS) $(FFTW3_LIBS) $(ROOT_LIBS) $(GSL_LIBS)
libPPhotoMeissner_la_LDFLAGS = -version-info $(PLUGIN_LIBRARY_VERSION) -release $(PLUGIN_RELEASE) $(AM_LDFLAGS)

## For the moment do not build pkgconfig files for musrfit plug-ins...
## pkgconfigdir = $(libdir)/pkgconfig
## pkgconfig_DATA = PPhotoMeissner.pc

## However, create some symbolic links to the shared library
## in order to unify the function call on different operating systems

if IS_DARWIN
install-exec-hook:
	$(LN_S) -f $(libdir)/libPPhotoMeissner.dylib $(libdir)/libPPhotoMeissner.so
uninstall-hook:
	rm -f $(libdir)/libPPhotoMeissner.so
endif

if IS_CYGWIN
install-exec-hook:
	$(LN_S) -f $(bindir)/cygPPhotoMeissner-$(PLUGIN_MAJOR_VERSION)-$(PLUGIN_MINOR_VERSION)-$(PLUGIN_MAJOR_VERSION).dll $(libdir)/libPSpinValve.so
uninstall-hook:
	rm -f $(libdir)/libPPhotoMeissner.so
endif

# define default xml install
xmldir = $(HOME)/.musrfit/external
xml_DATA = ../test/photoMeissner_startup.xml

instal-xml_DATA: $(xml_DATA)
	$(INSTALL_DATA) '$(xml_DATA)' '$(xmldir)' 

uninstal-xml_DATA: $(xml_DATA)
	$(RM) '$(xml_DATA)' '$(xmldir)' 

