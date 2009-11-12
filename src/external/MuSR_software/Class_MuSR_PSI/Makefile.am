## Process this file with automake to create Makefile.in

h_sources = MuSR_td_PSI_bin.h
cpp_sources = MuSR_td_PSI_bin.cpp 

include_HEADERS = MuSR_td_PSI_bin.h

INCLUDES = -I.
AM_CXXFLAGS = $(LOCAL_PSIBIN_LIB_CXXFLAGS)

AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS)
CLEANFILES = *~ core

lib_LTLIBRARIES = libClass_MuSR_PSI.la

libClass_MuSR_PSI_la_SOURCES = $(h_sources) $(cpp_sources)
libClass_MuSR_PSI_la_LDFLAGS = $(AM_LDFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = Class_MuSR_PSI.pc

