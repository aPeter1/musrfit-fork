## Process this file with automake to create Makefile.in

c_sources = Vegas.c

AM_CPPFLAGS = -I. -I.. -I../common -DNOUNDERSCORE
AM_CFLAGS = $(LOCAL_CUBA_LIB_CFLAGS) "-Dcubafun_=libVegascubafun_"
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS)

noinst_LTLIBRARIES = libvegas.la

libvegas_la_SOURCES = $(c_sources)
libvegas_la_LDFLAGS = $(AM_LDFLAGS)
