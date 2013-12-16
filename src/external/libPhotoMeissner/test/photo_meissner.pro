#  photo_meissner.pro
#  Author: Andreas Suter
#  e-mail: andreas.suter@psi.ch
# $Id$

SOURCES = photo_meissner.cpp

TARGET = photo_meissner

CONFIG = console debug

INCLUDEPATH += /usr/include/gsl

LIBS += -L "/usr/lib64" -lgsl -lgslcblas

