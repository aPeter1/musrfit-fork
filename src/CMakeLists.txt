#--- add further sub-directories ----------------------------------------------
#[==[
add_subdirectory(classes)
#]==]
add_subdirectory(external)
#[==[
if (Qt5Core_FOUND OR Qt4_FOUND)
  add_subdirectory(musredit)
endif (Qt5Core_FOUND OR Qt4_FOUND)
if (QT_FOUND)
  add_subdirectory(musrgui)
endif (QT_FOUND)
#]==]

#--- define the musrfit libs --------------------------------------------------
#[==[
set(MUSRFIT_LIBS ${MUSRFIT_LIBS} mud)
set(MUSRFIT_LIBS ${MUSRFIT_LIBS} TMusrRunHeader)
set(MUSRFIT_LIBS ${MUSRFIT_LIBS} TLemRunHeader)
set(MUSRFIT_LIBS ${MUSRFIT_LIBS} Class_MuSR_PSI)
if (nexus)
  set(MUSRFIT_LIBS ${MUSRFIT_LIBS} ${LIBNEXUS_LIBRARY})
  set(MUSRFIT_LIBS ${MUSRFIT_LIBS} PNeXus)
endif (nexus)
set(MUSRFIT_LIBS ${MUSRFIT_LIBS} PMusr)
#]==]

