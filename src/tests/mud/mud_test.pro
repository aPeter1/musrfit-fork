#------------------------------------------------------
# mud_test.pro
# qmake file for mud_test
#
# Andreas Suter, 2009/09/25
#
# $Id$
#
#------------------------------------------------------

fln = mud_test


MAKEFILE = Makefile

#QMAKE_CXXFLAGS_DEBUG += -O2
#QMAKE_CFLAGS_DEBUG += -O2

CONFIG += warn_on console debug

HEADERS = ./mud/src/mud.h

SOURCES = ./mud/src/mud_friendly.c \
          ./mud/src/mud.c \
	  ./mud/src/mud_new.c \
	  ./mud/src/mud_gen.c \
	  ./mud/src/mud_encode.c \
	  ./mud/src/mud_all.c \
	  ./mud/src/mud_tri_ti.c \
          $${fln}.c

TARGET=$${fln}
