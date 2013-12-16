## $Id$

if BUILD_ASLIBS
  ASDIRS = Nonlocal \
           MagProximity \
	   libSpinValve \
	   libPhotoMeissner
endif

if BUILD_CUBALIB
  CUBADIRS = libCuba
endif

if BUILD_BMWLIBS
  BMWDIRS = BMWtools \
            libFitPofB \
            libLFRelaxation \
            libZFRelaxation \
            libGapIntegrals \
            libCalcMeanFieldsLEM
endif

SUBDIRS = $(ASDIRS) $(CUBADIRS) $(BMWDIRS)
