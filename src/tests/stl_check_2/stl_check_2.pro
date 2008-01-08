#------------------------------------------------------
# stl_check_2.pro
# qmake file for stl_check_2
#
# Andreas Suter, 2007/05/14
#
# $Id$
#
#------------------------------------------------------

MAKEFILE = Makefile.stl_check_2

CONFIG += warn_on debug

HEADERS = PStlCheck.h \
          PPointObj.h

SOURCES = stl_check_2.cpp \
          PStlCheck.cpp \
          PPointObj.cpp

TARGET=stl_check_2
