# cmake file for the Meissner profile, assuming a photo induced
# exponential decaying additional superfluid density.
#
# Author: Andreas Suter
# e-mail: andreas.suter@psi.ch
#
# $Id$

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(photo_meissner)

ADD_EXECUTABLE(photo_meissner photo_meissner.cpp)


# look for GSL lib
FIND_LIBRARY( GSL_LIBRARY
  NAMES gsl
  PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64
)
# look for GSLCBLAS lib
FIND_LIBRARY( GSLCBLAS_LIBRARY
  NAMES gslcblas
  PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64
)

IF (GSL_LIBRARY)
  TARGET_LINK_LIBRARIES(photo_meissner ${GSL_LIBRARY})
ENDIF (GSL_LIBRARY)

IF (GSLCBLAS_LIBRARY)
  TARGET_LINK_LIBRARIES(photo_meissner ${GSLCBLAS_LIBRARY})
ENDIF (GSLCBLAS_LIBRARY)
