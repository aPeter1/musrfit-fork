#------------------------------------------------------
# dynGaussKT_LF.pro
# qmake file for dynGaussKT_LF
#
# Andreas Suter, 2009/01/13
#
# $Id$
#
#------------------------------------------------------

MAKEFILE = Makefile

CONFIG -= qt
CONFIG += warn_on console debug

SOURCES = dynGaussKT_LF.cpp

TARGET=dynGaussKT_LF

unix:LIBS += -lgsl -lgslcblas -lm
