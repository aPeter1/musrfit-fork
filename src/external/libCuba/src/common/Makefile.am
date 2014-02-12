## Process this file with automake to create Makefile.in

c_sources = WorkerIni.c

AM_CPPFLAGS = -I. -I.. -I../common -DNOUNDERSCORE
AM_CFLAGS = $(LOCAL_CUBA_LIB_CFLAGS)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS)

noinst_LTLIBRARIES = libworkerini.la

libworkerini_la_SOURCES = $(c_sources)
libworkerini_la_LDFLAGS = $(AM_LDFLAGS)
