## Process this file with automake to create Makefile.in

SUBDIRS = external/TLemRunHeader \
          external/MuSR_software \
          external/mud \
          classes \
          external

bin_PROGRAMS = musrfit musrview musrt0 musrparam msr2msr msr2data
musrfit_SOURCES = musrfit.cpp
musrview_SOURCES = musrview.cpp
musrt0_SOURCES = musrt0.cpp
musrparam_SOURCES = musrparam.cpp
msr2msr_SOURCES = msr2msr.cpp
msr2data_SOURCES = msr2data.cpp

xmldir = $(bindir)
xml_DATA = musrfit_startup.xml

LIBADD = $(PMUSR_LIBS) $(LEM_LIBS) $(PSIBIN_LIBS) $(MUD_LIBS)

AM_CXXFLAGS = $(LOCAL_BIN_CXXFLAGS)
AM_LDFLAGS = $(LOCAL_BIN_LDFLAGS)
INCLUDES = $(PMUSR_CFLAGS) $(FFTW3_CFLAGS) $(GSL_CFLAGS) $(BOOST_CFLAGS) $(ROOT_CFLAGS)
LIBS = $(PMUSR_LIBS) $(LEM_LIBS) $(PSIBIN_LIBS) $(MUD_LIBS) $(FFTW3_LIBS) $(GSL_LIBS) $(ROOT_LIBS)

