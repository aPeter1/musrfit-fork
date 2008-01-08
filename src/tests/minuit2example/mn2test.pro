#------------------------------------------------------
# mn2test.pro
# qmake file for mn2test
#
# Andreas Suter, 2007/10/23
#
# $Id$
#
#------------------------------------------------------

MAKEFILE = Makefile.mn2test

CONFIG += warn_on debug

HEADERS = PGlobalChiSquare.h

SOURCES = mn2test.cpp \
          PGlobalChiSquare.cpp

INCLUDEPATH += /usr/local/include
INCLUDEPATH += ./

unix:LIBS += -L/usr/local/lib -lMinuit2Base -lbsd -lm -ldl -lutil

TARGET=mn2test

