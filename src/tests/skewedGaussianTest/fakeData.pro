#------------------------------------------------------
# fakeData.pro
# qmake file for fakeData
#
# Andreas Suter, 2008/02/12
#
# $Id$
#
#------------------------------------------------------

MAKEFILE = Makefile.fakeData

CONFIG += warn_on debug

SOURCES = fakeData.cpp \

INCLUDEPATH += $$(ROOTSYS)/include

ROOTLIBS = -L$$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --libs)
NEMULIBS = -lTLemRunHeader
unix:LIBS += -L$$(ROOTSYS)/lib $${ROOTLIBS} $${NEMULIBS} -lbsd -lm -ldl -lutil

TARGET = fakeData
