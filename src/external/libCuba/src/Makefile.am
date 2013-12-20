## Process this file with automake to create Makefile.in
## $Id$

SUBDIRS = cuhre divonne suave vegas common

include_HEADERS = cuba.h

AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS)
CLEANFILES = common/*~ cuhre/*~ divonne/*~ suave/*~ vegas/*~ *~ core

lib_LTLIBRARIES = libcuba.la

libcuba_la_SOURCES =

libcuba_la_LIBADD = cuhre/libcuhre.la divonne/libdivonne.la suave/libsuave.la vegas/libvegas.la -lm
libcuba_la_LDFLAGS = -version-info $(CUBA_LIBRARY_VERSION) -release $(CUBA_RELEASE) $(AM_LDFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cuba.pc

