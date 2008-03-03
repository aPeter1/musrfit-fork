#------------------------------------------------------
# fakeDataNemu.pro
# qmake file for fakeDataNemu
#
# Andreas Suter, 2008/02/12
#
# $Id$
#
#------------------------------------------------------

MAKEFILE = Makefile.fakeDataNemu

CONFIG += warn_on debug

SOURCES = fakeDataNemu.cpp \

INCLUDEPATH += $$(ROOTSYS)/include

ROOTLIBS = -L$$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --libs)
unix:LIBS += -L$$(ROOTSYS)/lib $${ROOTLIBS} -lbsd -lm -ldl -lutil

TARGET = fakeDataNemu
