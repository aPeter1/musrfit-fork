## Process this file with automake to create Makefile.in
## $Id$

h_sources = \
  PNeXus.h

cpp_sources = \
  PNeXus.cpp

include_HEADERS = $(h_sources)

INCLUDES = $(HDF5_CFLAGS) $(NEXUS_CFLAGS)
AM_CXXFLAGS = $(LOCAL_PNEXUS_LIB_CXXFLAGS)

AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS)
CLEANFILES = *~ core

lib_LTLIBRARIES = libPNeXus.la

libPNeXus_la_SOURCES = $(h_sources) $(cpp_sources) 
libPNeXus_la_LIBADD = $(NEXUS_LIBS)
libPNeXus_la_LDFLAGS = -version-info $(PNEXUS_LIBRARY_VERSION) -release $(PNEXUS_RELEASE) $(AM_LDFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = PNeXus.pc

