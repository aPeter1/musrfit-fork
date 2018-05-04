#--- build the PMusr and PUserFcnBase libs ------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
root_generate_dictionary(
  PFourierCanvasDict ${MUSRFIT_INC}/PFourierCanvas.h LINKDEF ${MUSRFIT_INC}/PFourierCanvasLinkDef.h
)
root_generate_dictionary(
  PMusrCanvasDict ${MUSRFIT_INC}/PMusrCanvas.h LINKDEF ${MUSRFIT_INC}/PMusrCanvasLinkDef.h
)
root_generate_dictionary(
  PMusrT0Dict ${MUSRFIT_INC}/PMusrT0.h LINKDEF ${MUSRFIT_INC}/PMusrT0LinkDef.h
)
root_generate_dictionary(
  PStartupHandlerDict ${MUSRFIT_INC}/PStartupHandler.h LINKDEF ${MUSRFIT_INC}/PStartupHandlerLinkDef.h
)
root_generate_dictionary(
  PUserFcnBaseDict ${MUSRFIT_INC}/PUserFcnBase.h LINKDEF ${MUSRFIT_INC}/PUserFcnBaseLinkDef.h
)

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(MUSR_VERSION "1.2.0")
set(MUSR_LIBRARY_NAME "PMusr")
configure_file("PMusr.pc.in" "PMusr.pc" @ONLY)
set(USERFCN_LIBRARY_NAME "PUserFcnBase")
configure_file("PUserFcnBase.pc.in" "PUserFcnBase.pc" @ONLY)

#--- lib creation -------------------------------------------------------------
add_library(PMusr SHARED
  PFitter.cpp
  PFitterFcn.cpp
  PFourier.cpp
  PFourierCanvas.cpp
  PFourierCanvasDict.cxx
  PMusrCanvas.cpp
  PMusrCanvasDict.cxx
  PFunction.cpp
  PFunctionHandler.cpp
  PMsr2Data.cpp
  PMsrHandler.cpp
  PMusrCanvas.cpp
  PMusrCanvasDict.cxx
  PMusr.cpp
  PMusrT0.cpp
  PMusrT0Dict.cxx
  PPrepFourier.cpp
  PRunAsymmetry.cpp
  PRunAsymmetryRRF.cpp
  PRunBase.cpp
  PRunDataHandler.cpp
  PRunListCollection.cpp
  PRunMuMinus.cpp
  PRunNonMusr.cpp
  PRunSingleHisto.cpp
  PRunSingleHistoRRF.cpp
  PStartupHandler.cpp
  PStartupHandlerDict.cxx
  PTheory.cpp
)

add_library(PUserFcnBase SHARED
  PUserFcnBase.cpp
  PUserFcnBaseDict.cxx
)

#--- add library dependencies -------------------------------------------------
set(DependOnLibs ${ROOT_LIBARIES})
set(DependOnLibs ${DependOnLibs} ${FFTW3_LIBRARY})
set(DependOnLibs ${DependOnLibs} PUserFcnBase)
set(DependOnLibs ${DependOnLibs} mud)
set(DependOnLibs ${DependOnLibs} TMusrRunHeader)
set(DependOnLibs ${DependOnLibs} TLemRunHeader)
set(DependOnLibs ${DependOnLibs} Class_MuSR_PSI)
if (nexus)
  set(DependOnLibs ${DependOnLibs} ${LIBNEXUS_LIBRARY})
  set(DependOnLibs ${DependOnLibs} PNeXus)
endif (nexus)

target_link_libraries(PUserFcnBase ${ROOT_LIBRARIES})
target_link_libraries(PMusr ${DependOnLibs})

#--- install PUserFcnBase solib -----------------------------------------------
install(TARGETS PUserFcnBase DESTINATION lib)

#--- install PMusr solib ------------------------------------------------------
install(TARGETS PMusr DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libPFourierCanvasDict_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libPFourierCanvasDict.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libPMusrCanvasDict_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libPMusrCanvasDict.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libPMusrT0Dict_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libPMusrT0Dict.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libPStartupHandlerDict_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libPStartupHandlerDict.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libPUserFcnBaseDict_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libPUserFcnBaseDict.rootmap
  DESTINATION lib
)

#--- install headers ----------------------------------------------------------
#--- only install headers needed for classes with dictionary
install(
  FILES ${MUSRFIT_INC}/PFourierCanvas.h
        ${MUSRFIT_INC}/PFourier.h 
        ${MUSRFIT_INC}/PMusrCanvas.h
        ${MUSRFIT_INC}/PMusr.h
        ${MUSRFIT_INC}/PMusrT0.h
        ${MUSRFIT_INC}/PRunListCollection.h
        ${MUSRFIT_INC}/PStartupHandler.h
        ${MUSRFIT_INC}/PUserFcnBase.h
  DESTINATION include
)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/PUserFcnBase.pc
        ${CMAKE_CURRENT_BINARY_DIR}/PMusr.pc
  DESTINATION lib/pkgconfig
)

