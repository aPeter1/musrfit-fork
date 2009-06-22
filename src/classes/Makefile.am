## Process this file with automake to create Makefile.in

h_sources = \
  ../include/PFitterFcn.h \
  ../include/PFitter.h \
  ../include/PFourier.h \
  ../include/PFunctionGrammar.h \
  ../include/PFunction.h \
  ../include/PFunctionHandler.h \
  ../include/PMsrHandler.h \
  ../include/PMusrCanvas.h \
  ../include/PMusr.h \
  ../include/PMusrT0.h \
  ../include/PRunAsymmetry.h \
  ../include/PRunBase.h \
  ../include/PRunDataHandler.h \
  ../include/PRunListCollection.h \
  ../include/PRunNonMusr.h \
  ../include/PRunRRF.h \
  ../include/PRunSingleHisto.h \
  ../include/PStartupHandler.h \
  ../include/PTheory.h \
  ../include/PUserFcnBase.h 

h_linkdef = \
  ../include/PMusrCanvasLinkDef.h \
  ../include/PMusrT0LinkDef.h \
  ../include/PStartupHandlerLinkDef.h \
  ../include/PUserFcnBaseLinkDef.h

dict_h_sources = \
  PMusrCanvasDict.h \
  PMusrT0Dict.h \
  PStartupHandlerDict.h \
  PUserFcnBaseDict.h

cpp_sources = \
  PFitter.cpp \
  PFitterFcn.cpp \
  PFourier.cpp \
  PFunction.cpp \
  PFunctionHandler.cpp \
  PMsrHandler.cpp \
  PMusrCanvas.cpp \
  PMusrT0.cpp \
  PRunAsymmetry.cpp \
  PRunBase.cpp \
  PRunDataHandler.cpp \
  PRunListCollection.cpp \
  PRunNonMusr.cpp \
  PRunRRF.cpp \
  PRunSingleHisto.cpp \
  PStartupHandler.cpp \
  PTheory.cpp \
  PUserFcnBase.cpp

dict_cpp_sources = \
  PMusrCanvasDict.cpp \
  PMusrT0Dict.cpp \
  PStartupHandlerDict.cpp \
  PUserFcnBaseDict.cpp

ext_source_dir = $(top_srcdir)/src/external/MuSR_software/Class_MuSR_PSI
ext_h_sources = $(ext_source_dir)/MuSR_td_PSI_bin.h $(ext_source_dir)/tydefs.h
ext_cpp_sources = $(ext_source_dir)/MuSR_td_PSI_bin.cpp 

include_HEADERS = $(h_sources) $(ext_source_dir)/MuSR_td_PSI_bin.h
noinst_HEADERS = $(h_linkdef) $(dict_h_sources) $(ext_source_dir)/tydefs.h

INCLUDES = -I$(top_srcdir)/src/include -I$(ext_source_dir) $(LEM_CFLAGS) $(FFTW3_CFLAGS) $(GSL_CFLAGS) $(BOOST_CFLAGS) $(ROOT_CFLAGS)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *~ core

%Dict.cpp %Dict.h: ../include/%.h ../include/%LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cpp -c -p $(INCLUDES) $^

lib_LTLIBRARIES = libPMusr.la

libPMusr_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources) $(ext_h_sources) $(ext_cpp_sources)
libPMusr_la_LIBADD = $(LEM_LIBS) $(FFTW3_LIBS) $(GSL_LIBS) $(ROOT_LIBS)
libPMusr_la_LDFLAGS = -version-info $(MUSR_LIBRARY_VERSION) -release $(MUSR_RELEASE) $(AM_LDFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = PMusr.pc

