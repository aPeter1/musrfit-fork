#------------------------------------------------------
# stl_check.pro
# qmake file for stl_check
#
# Andreas Suter, 2007/05/14
#
# $Id$
#
#------------------------------------------------------

MAKEFILE = Makefile.stl_check

CONFIG += warn_on debug

HEADERS = PStlCheck.h \
          PPointObj.h

SOURCES = stl_check.cpp \
          PStlCheck.cpp \
          PPointObj.cpp

TARGET=stl_check
