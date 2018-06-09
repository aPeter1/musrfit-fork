## Process this file with automake to create Makefile.in

c_sources = \
  Global.c \
  Data.c

AM_CPPFLAGS = -I. -I.. -I../common -DNOUNDERSCORE
AM_CFLAGS = $(LOCAL_CUBA_LIB_CFLAGS)
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS)

noinst_LTLIBRARIES = libcommon.la

libcommon_la_SOURCES = $(c_sources)
libcommon_la_LDFLAGS = $(AM_LDFLAGS)
