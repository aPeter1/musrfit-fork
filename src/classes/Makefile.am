## Process this file with automake to create Makefile.in

h_sources = \
  ../include/PFitterFcn.h \
  ../include/PFitter.h \
  ../include/PFourier.h \
  ../include/PFunctionGrammar.h \
  ../include/PFunction.h \
  ../include/PFunctionHandler.h \
  ../include/PMsr2Data.h \
  ../include/PMsrHandler.h \
  ../include/PMusrCanvas.h \
  ../include/PMusr.h \
  ../include/PMusrT0.h \
  ../include/PRunAsymmetry.h \
  ../include/PRunBase.h \
  ../include/PRunDataHandler.h \
  ../include/PRunListCollection.h \
  ../include/PRunNonMusr.h \
  ../include/PRunMuMinus.h \
  ../include/PRunSingleHisto.h \
  ../include/PStartupHandler.h \
  ../include/PTheory.h

h_sources_userFcn = \
  ../include/PUserFcnBase.h

h_linkdef = \
  ../include/PMusrCanvasLinkDef.h \
  ../include/PMusrT0LinkDef.h \
  ../include/PStartupHandlerLinkDef.h

h_linkdef_userFcn = \
  ../include/PUserFcnBaseLinkDef.h

dict_h_sources = \
  PMusrCanvasDict.h \
  PMusrT0Dict.h \
  PStartupHandlerDict.h

dict_h_sources_userFcn = \
  PUserFcnBaseDict.h

cpp_sources = \
  PFitter.cpp \
  PFitterFcn.cpp \
  PFourier.cpp \
  PFunction.cpp \
  PFunctionHandler.cpp \
  PMsr2Data.cpp \
  PMsrHandler.cpp \
  PMusrCanvas.cpp \
  PMusr.cpp \
  PMusrT0.cpp \
  PRunAsymmetry.cpp \
  PRunBase.cpp \
  PRunDataHandler.cpp \
  PRunListCollection.cpp \
  PRunNonMusr.cpp \
  PRunMuMinus.cpp \
  PRunSingleHisto.cpp \
  PStartupHandler.cpp \
  PTheory.cpp

cpp_sources_userFcn = \
  PUserFcnBase.cpp

dict_cpp_sources = \
  PMusrCanvasDict.cpp \
  PMusrT0Dict.cpp \
  PStartupHandlerDict.cpp

dict_cpp_sources_userFcn = \
  PUserFcnBaseDict.cpp

include_HEADERS = $(h_sources) $(h_sources_userFcn)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources) $(h_linkdef_userFcn) $(dict_h_sources_userFcn)

INCLUDES = -I$(top_srcdir)/src/include $(PSIBIN_CFLAGS) $(MUD_CFLAGS) $(LEM_CFLAGS) $(FFTW3_CFLAGS) $(GSL_CFLAGS) $(BOOST_CFLAGS) $(ROOT_CFLAGS) $(PNEXUS_CXXFLAGS) $(NEXUS_CFLAGS)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources) $(dict_cpp_sources_userFcn) $(dict_h_sources_userFcn)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *~ core

%Dict.cpp %Dict.h: ../include/%.h ../include/%LinkDef.h
	@ROOTCINT@ -v -f $*Dict.cpp -c -p $(INCLUDES) $^

lib_LTLIBRARIES = libPMusr.la libPUserFcnBase.la

libPMusr_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libPMusr_la_LIBADD = libPUserFcnBase.la $(LEM_LIBS) $(PSIBIN_LIBS) $(MUD_LIBS) $(PNEXUS_LIBS) $(FFTW3_LIBS) $(GSL_LIBS) $(ROOT_LIBS)
libPMusr_la_LDFLAGS = -version-info $(MUSR_LIBRARY_VERSION) -release $(MUSR_RELEASE) $(AM_LDFLAGS)

libPUserFcnBase_la_SOURCES = $(h_sources_userFcn) $(cpp_sources_userFcn) $(dict_h_sources_userFcn) $(dict_cpp_sources_userFcn)
libPUserFcnBase_la_LIBADD = $(ROOT_LIBS)
libPUserFcnBase_la_LDFLAGS = -version-info $(MUSR_LIBRARY_VERSION) -release $(MUSR_RELEASE) $(AM_LDFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = PMusr.pc PUserFcnBase.pc

