## Process this file with automake to create Makefile.in

h_sources = mud.h
cpp_sources = mud_all.c \
              mud.c \
	      mud_encode.c \
	      mud_friendly.c \
	      mud_gen.c \
  	      mud_new.c \
	      mud_tri_ti.c

include_HEADERS = mud.h

INCLUDES = -I.
AM_CXXFLAGS = $(LOCAL_MUD_LIB_CXXFLAGS)

AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS)
CLEANFILES = *~ core

lib_LTLIBRARIES = libmud.la

libmud_la_SOURCES = $(h_sources) $(cpp_sources)
libmud_la_LDFLAGS = $(AM_LDFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mud.pc

