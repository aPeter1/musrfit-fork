#------------------------------------------------------
# spirit_fcn_test.pro
# qmake file for spirit_fcn_test
#
# Andreas Suter, 2007/12/10
#
# $Id$
#
#------------------------------------------------------

MAKEFILE = Makefile

CONFIG += warn_on debug

HEADERS = PFunctionGrammar.h \
          PFunction.h \
          PFunctionHandler.h

SOURCES = spirit_fcn_test.cpp \
          PFunction.cpp \
          PFunctionHandler.cpp
 
TARGET=spirit_fcn_test

