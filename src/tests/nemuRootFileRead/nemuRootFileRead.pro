#------------------------------------------------------
# nemuRootFileRead.pro
# qmake file for rootSystemTest
#
# Andreas Suter, 2007/05/14
#
# $Id$
#
#------------------------------------------------------

MAKEFILE = Makefile

CONFIG += warn_on debug

SOURCES = nemuRootFileRead.cpp \

INCLUDEPATH += $$(ROOTSYS)/include

ROOTLIBS = -lCore -lCint -lRIO -lGraf -lGraf3d -lGpad -lHist -lMatrix -lMinuit -lGui
NEMULIBS = -lTLemRunHeader
unix:LIBS += -L$$(ROOTSYS)/lib $${ROOTLIBS} $${NEMULIBS} -lbsd -lm -ldl -lutil

TARGET=nemuRootFileRead
