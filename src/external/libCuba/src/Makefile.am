## Process this file with automake to create Makefile.in
## $Id$

h_sources = cuba.h

c_sources = cuhre/Cuhre.c \
            divonne/Divonne.c \
            suave/Suave.c \
            vegas/Vegas.c

include_HEADERS = cuba.h

INCLUDES = -I. -Icommon
AM_CFLAGS = $(LOCAL_CUBA_LIB_CFLAGS)

AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS)
CLEANFILES = common/*~ cuhre/*~ divonne/*~ suave/*~ vegas/*~ *~ core

lib_LTLIBRARIES = libcuba.la

libcuba_la_SOURCES = $(h_sources) $(c_sources)
libcuba_la_LIBADD = -lm
libcuba_la_LDFLAGS = -version-info $(CUBA_LIBRARY_VERSION) -release $(CUBA_RELEASE) $(AM_LDFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cuba.pc

