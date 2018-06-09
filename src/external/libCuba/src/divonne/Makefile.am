## Process this file with automake to create Makefile.in

c_sources = Divonne.c

AM_CPPFLAGS = -I. -I.. -I../common -DNOUNDERSCORE
AM_CFLAGS = $(LOCAL_CUBA_LIB_CFLAGS) "-Dcubafun_=libDivonnecubafun_"
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS)

noinst_LTLIBRARIES = libdivonne.la

libdivonne_la_SOURCES = $(c_sources)
libdivonne_la_LDFLAGS = $(AM_LDFLAGS)
