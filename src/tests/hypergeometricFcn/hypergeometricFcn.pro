#------------------------------------------------------
# hypergeometricFcn.pro
# qmake file for hypergeometricFcn
#
# Andreas Suter, 2008/02/12
#
# $Id$
#
#------------------------------------------------------

MAKEFILE = Makefile

CONFIG += warn_on debug

HEADERS =

SOURCES = hypergeometricFcn.cpp

INCLUDEPATH += /usr/include/gsl

unix:LIBS += -lgslcblas -lgsl -lbsd -lm -ldl -lutil

TARGET = hypergeometricFcn

