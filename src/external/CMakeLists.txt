if (ASlibs)
  add_subdirectory(Nonlocal)
  add_subdirectory(MagProximity)
  add_subdirectory(libPhotoMeissner)
#[==[
  add_subdirectory(libSpinValve)
#]==]
endif (ASlibs)

#[==[
if (BMWlibs)
  add_subdirectory(libCuba)
  add_subdirectory(BMWtools)
  add_subdirectory(libCalcMeanFieldsLEM)
  add_subdirectory(libFitPofB)
  add_subdirectory(libGapIntegrals)
  add_subdirectory(libLFRelaxation)
  add_subdirectory(libZFRelaxation)
endif (BMWlibs)
#]==]

add_subdirectory(mud)
add_subdirectory(MusrRoot)
add_subdirectory(TLemRunHeader)
add_subdirectory(MuSR_software)

if (nexus)
  add_subdirectory(nexus)
endif (nexus)

