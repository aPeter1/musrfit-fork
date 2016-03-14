
if BUILD_ASLIBS
  ASDIRS = Nonlocal \
           MagProximity \
	   libSpinValve \
	   libPhotoMeissner \
           libGbGLF
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

if BUILD_BNMRLIBS
  BNMRDIRS = libBNMR
endif

SUBDIRS = $(ASDIRS) $(CUBADIRS) $(BMWDIRS) $(BNMRDIRS)
