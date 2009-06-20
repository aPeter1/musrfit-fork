## Process this file with automake to create Makefile.in

SUBDIRS = classes external

bin_PROGRAMS = musrfit musrview musrt0 musrparam msr2msr
musrfit_SOURCES = musrfit.cpp
musrview_SOURCES = musrview.cpp
musrt0_SOURCES = musrt0.cpp
musrparam_SOURCES = musrparam.cpp
msr2msr_SOURCES = msr2msr.cpp

xmldir = $(bindir)
xml_DATA = musrfit_startup.xml

LIBADD = $(LEM_LIBS) $(PMUSR_LIBS)

AM_CXXFLAGS = -Wall -Wno-trigraphs
INCLUDES = $(PMUSR_CFLAGS) $(FFTW3_CFLAGS) $(GSL_CFLAGS) $(BOOST_CFLAGS) $(ROOT_CFLAGS)
LIBS = $(LEM_LIBS) $(PMUSR_LIBS) $(FFTW3_LIBS) $(GSL_LIBS) $(ROOT_LIBS)

