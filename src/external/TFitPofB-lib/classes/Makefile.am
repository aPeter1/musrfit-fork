## Process this file with automake to create Makefile.in

h_sources = \
  ../include/TBofZCalc.h \
  ../include/TBulkTriVortexFieldCalc.h \
  ../include/TLondon1D.h \
  ../include/TPofBCalc.h \
  ../include/TPofTCalc.h \
  ../include/TSkewedGss.h \
  ../include/TTrimSPDataHandler.h \
  ../include/TVortex.h

h_linkdef = \
  ../include/TLondon1DLinkDef.h \
  ../include/TVortexLinkDef.h \
  ../include/TSkewedGssLinkDef.h

dict_h_sources = \
  TLondon1DDict.h \
  TSkewedGssDict.h \
  TVortexDict.h

cpp_sources = \
  TBulkTriVortexFieldCalc.cpp \
  TBofZCalc.cpp \
  TLondon1D.cpp \
  TPofBCalc.cpp \
  TPofTCalc.cpp \
  TSkewedGss.cpp \
  TTrimSPDataHandler.cpp \
  TVortex.cpp

dict_cpp_sources = \
  TLondon1DDict.cpp \
  TSkewedGssDict.cpp \
  TVortexDict.cpp

include_HEADERS = $(h_sources)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources)

INCLUDES = -I$(top_srcdir)/src/include -I../include $(BMWSTARTUP_CFLAGS) $(LEM_CFLAGS) $(PMUSR_CFLAGS) $(FFTW3_CFLAGS) $(ROOT_CFLAGS)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *~ core

%Dict.cpp %Dict.h: ../include/%.h ../include/%LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cpp -c -p $(INCLUDES) $^

lib_LTLIBRARIES = libTFitPofB.la

libTFitPofB_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libTFitPofB_la_LIBADD = $(BMWSTARTUP_LIBS) $(LEM_LIBS) $(PMUSR_LIBS) $(FFTW3_LIBS) $(ROOT_LIBS)
libTFitPofB_la_LDFLAGS = -version-info $(PLUGIN_LIBRARY_VERSION) -release $(PLUGIN_RELEASE) $(AM_LDFLAGS)

## For the moment do not build pkgconfig files for musrfit plug-ins...
## pkgconfigdir = $(libdir)/pkgconfig
## pkgconfig_DATA = PTFitPofB.pc

## However, create some symbolic links to the shared library
## in order to unify the function call on different operating systems

if IS_DARWIN
install-exec-hook:
	$(LN_S) $(libdir)/libTFitPofB.dylib $(libdir)/libTFitPofB.so
uninstall-hook:
	rm -f $(libdir)/libTFitPofB.so
endif

if IS_CYGWIN
install-exec-hook:
	$(LN_S) $(bindir)/cygTFitPofB-$(PLUGIN_MAJOR_VERSION)-$(PLUGIN_MINOR_VERSION)-$(PLUGIN_MAJOR_VERSION).dll $(libdir)/libTFitPofB.so
uninstall-hook:
	rm -f $(libdir)/libTFitPofB.so
endif
