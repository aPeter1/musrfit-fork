## Process this file with automake to create Makefile.in

c_sources = Cuhre.c

AM_CPPFLAGS = -I. -I.. -I../common -DNOUNDERSCORE
AM_CFLAGS = $(LOCAL_CUBA_LIB_CFLAGS) "-Dcubafun_=libCuhrecubafun_"
AM_LDFLAGS = $(LOCAL_LIB_LDFLAGS)

noinst_LTLIBRARIES = libcuhre.la

libcuhre_la_SOURCES = $(c_sources)
libcuhre_la_LDFLAGS = $(AM_LDFLAGS)
