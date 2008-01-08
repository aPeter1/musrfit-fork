#------------------------------------------------------
# rootSystemTest.pro
# qmake file for rootSystemTest
#
# Andreas Suter, 2007/05/14
#
# $Id$
#
#------------------------------------------------------

MAKEFILE = Makefile

CONFIG += warn_on debug

SOURCES = rootSystemTest.cpp \

INCLUDEPATH += $$(ROOTSYS)/include

ROOTLIBS = -lCore -lCint
unix:LIBS += -L$$(ROOTSYS)/lib $${ROOTLIBS} -lbsd -lm -ldl -lutil

TARGET=rootSystemTest
