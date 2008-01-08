#------------------------------------------------------
# musrfit.pro
# qmake file for musrfit
#
# Andreas Suter, 2007/05/14
#
# $Id$
#
#------------------------------------------------------

MAKEFILE = Makefile.musrfit

CONFIG += warn_on debug

SOURCES = musrfit.cpp \

INCLUDEPATH += $$(ROOTSYS)/include
INCLUDEPATH += ./include

PSILIBS    = -lTLemRunHeader -lPMusr
ROOTLIBS   = $(shell $(ROOTSYS)/bin/root-config --libs)
unix:LIBS += $${PSILIBS} 
unix:LIBS += $${ROOTLIBS} 

TARGET=musrfit
