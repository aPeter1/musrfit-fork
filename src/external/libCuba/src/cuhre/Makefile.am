## Process this file with automake to create Makefile.in
## $Id: Makefile.am 4808 2011-03-21 14:33:34Z l_wojek $

c_sources = Cuhre.c

AM_CPPFLAGS = -I. -I.. -I../common -DNOUNDERSCORE
AM_CFLAGS = $(LOCAL_CUBA_LIB_CFLAGS)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS)

noinst_LTLIBRARIES = libcuhre.la

libcuhre_la_SOURCES = $(c_sources)
libcuhre_la_LDFLAGS = $(AM_LDFLAGS)