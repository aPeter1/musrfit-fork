#------------------------------------------------------
# msr2msr.pro
# qmake file for msr2msr
#
# Andreas Suter, 2008/01/03
#
# $Id$
#
#------------------------------------------------------

MAKEFILE = Makefile.msr2msr

CONFIG += warn_on debug

SOURCES = msr2msr.cpp \

INCLUDEPATH += $$(ROOTSYS)/include

ROOTLIBS   = $(shell $(ROOTSYS)/bin/root-config --libs)
unix:LIBS += $${ROOTLIBS} 

TARGET=msr2msr
