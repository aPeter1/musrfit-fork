## Process this file with automake to create Makefile.in

h_sources = \
  ../include/PFitterFcn.h \
  ../include/PFitter.h \
  ../include/PFourier.h \
  ../include/PFourierCanvas.h \
  ../include/PFunctionGrammar.h \
  ../include/PFunction.h \
  ../include/PFunctionHandler.h \
  ../include/PMsr2Data.h \
  ../include/PMsrHandler.h \
  ../include/PMusrCanvas.h \
  ../include/PMusr.h \
  ../include/PMusrT0.h \
  ../include/PPrepFourier.h \
  ../include/PRunAsymmetry.h \
  ../include/PRunAsymmetryRRF.h \
  ../include/PRunBase.h \
  ../include/PRunDataHandler.h \
  ../include/PRunListCollection.h \
  ../include/PRunNonMusr.h \
  ../include/PRunMuMinus.h \
  ../include/PRunSingleHisto.h \
  ../include/PRunSingleHistoRRF.h \
  ../include/PStartupHandler.h \
  ../include/PTheory.h

h_sources_userFcn = \
  ../include/PUserFcnBase.h

h_linkdef = \
  ../include/PFourierCanvasLinkDef.h \
  ../include/PMusrCanvasLinkDef.h \
  ../include/PMusrT0LinkDef.h \
  ../include/PStartupHandlerLinkDef.h

h_linkdef_userFcn = \
  ../include/PUserFcnBaseLinkDef.h

dict_h_sources = \
  PFourierCanvasDict.h \
  PMusrCanvasDict.h \
  PMusrT0Dict.h \
  PStartupHandlerDict.h

dict_h_sources_userFcn = \
  PUserFcnBaseDict.h

cpp_sources = \
  PFitter.cpp \
  PFitterFcn.cpp \
  PFourier.cpp \
  PFourierCanvas.cpp \
  PFunction.cpp \
  PFunctionHandler.cpp \
  PMsr2Data.cpp \
  PMsrHandler.cpp \
  PMusrCanvas.cpp \
  PMusr.cpp \
  PMusrT0.cpp \
  PPrepFourier.cpp \
  PRunAsymmetry.cpp \
  PRunAsymmetryRRF.cpp \
  PRunBase.cpp \
  PRunDataHandler.cpp \
  PRunListCollection.cpp \
  PRunNonMusr.cpp \
  PRunMuMinus.cpp \
  PRunSingleHisto.cpp \
  PRunSingleHistoRRF.cpp \
  PStartupHandler.cpp \
  PTheory.cpp

cpp_sources_userFcn = \
  PUserFcnBase.cpp

dict_cpp_sources = \
  PFourierCanvasDict.cpp \
  PMusrCanvasDict.cpp \
  PMusrT0Dict.cpp \
  PStartupHandlerDict.cpp

dict_cpp_sources_userFcn = \
  PUserFcnBaseDict.cpp

pcmdir = $(libdir)
pcm_DATA = \
  PFourierCanvasDict_rdict.pcm \
  PMusrCanvasDict_rdict.pcm \
  PMusrT0Dict_rdict.pcm \
  PStartupHandlerDict_rdict.pcm \
  PUserFcnBaseDict_rdict.pcm

include_HEADERS = $(h_sources) $(h_sources_userFcn)
noinst_HEADERS = $(h_linkdef) $(dict_h_sources) $(h_linkdef_userFcn) $(dict_h_sources_userFcn)

AM_CPPFLAGS = -I$(top_srcdir)/src/include $(MUSR_ROOT_CFLAGS) $(PSIBIN_CFLAGS) $(MUD_CFLAGS) $(LEM_CFLAGS) $(FFTW3_CFLAGS) $(GSL_CFLAGS) $(BOOST_CFLAGS) -I$(ROOTINCDIR) $(PNEXUS_CXXFLAGS) $(NEXUS_CFLAGS)
AM_CXXFLAGS = $(LOCAL_LIB_CXXFLAGS)

BUILT_SOURCES = $(dict_cpp_sources) $(dict_h_sources) $(dict_cpp_sources_userFcn) $(dict_h_sources_userFcn)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS) -L@ROOTLIBDIR@
CLEANFILES = *Dict.cpp *Dict.h *Dict* *~ core

%Dict.cpp %Dict.h: ../include/%.h ../include/%LinkDef.h
	@ROOTCLING@ -v -f $*Dict.cpp -c -p $(AM_CPPFLAGS) $^

lib_LTLIBRARIES = libPUserFcnBase.la libPMusr.la

libPUserFcnBase_la_SOURCES = $(h_sources_userFcn) $(cpp_sources_userFcn) $(dict_h_sources_userFcn) $(dict_cpp_sources_userFcn)
libPUserFcnBase_la_LIBADD = $(ROOT_LIBS)
libPUserFcnBase_la_LDFLAGS = -version-info $(MUSR_LIBRARY_VERSION) -release $(MUSR_RELEASE) $(AM_LDFLAGS)

libPMusr_la_SOURCES = $(h_sources) $(cpp_sources) $(dict_h_sources) $(dict_cpp_sources)
libPMusr_la_LIBADD = libPUserFcnBase.la $(MUSR_ROOT_LIBS) $(LEM_LIBS) $(PSIBIN_LIBS) $(MUD_LIBS) $(PNEXUS_LIBS) $(FFTW3_LIBS) $(GSL_LIBS) $(ROOT_LIBS)
libPMusr_la_LDFLAGS = -version-info $(MUSR_LIBRARY_VERSION) -release $(MUSR_RELEASE) $(AM_LDFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = PUserFcnBase.pc PMusr.pc

