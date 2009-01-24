#------------------------------------------------------
# dynKT_LF.pro
# qmake file for dynKT_LF
#
# Andreas Suter, 2009/01/13
#
# $Id$
#
#------------------------------------------------------

MAKEFILE = Makefile

CONFIG -= qt
CONFIG += warn_on console debug

SOURCES = dynKT_LF.cpp

TARGET=dynKT_LF

unix:LIBS += -lgsl -lgslcblas -lm
