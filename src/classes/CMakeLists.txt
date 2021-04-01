#--- build the PMusr, PUserFcnBase, and PRgeHandler libs ----------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
# ROOT requires that the dictonary header files are found at configuration time.
# Hence, target_include_directories cannot be used here because, targets are 
# setup only afterwards.
include_directories(${MUSRFIT_INC})

root_generate_dictionary(
  PFourierCanvasDict 
  PFourierCanvas.h 
  OPTIONS 
    -I${MUSRFIT_INC} 
    -I${FFTW3_INCLUDE_DIR}
    -inlineInputHeader
  LINKDEF ${MUSRFIT_INC}/PFourierCanvasLinkDef.h
  MODULE PFourierCanvas
)
root_generate_dictionary(
  PMusrCanvasDict
  PMusrCanvas.h  
  LINKDEF ${MUSRFIT_INC}/PMusrCanvasLinkDef.h
  OPTIONS 
    -I${Boost_INCLUDE_DIR} -I${FFTW3_INCLUDE_DIR} -I${MUSRFIT_INC}
    -inlineInputHeader
  MODULE PMusrCanvas
)
root_generate_dictionary(
  PMusrT0Dict 
  PMusrT0.h
  LINKDEF ${MUSRFIT_INC}/PMusrT0LinkDef.h
  OPTIONS 
    -I${Boost_INCLUDE_DIR} -I${FFTW3_INCLUDE_DIR} -I${MUSRFIT_INC}
    -inlineInputHeader
  MODULE PMusrT0
)
root_generate_dictionary(
  PStartupHandlerDict 
  PStartupHandler.h 
  LINKDEF ${MUSRFIT_INC}/PStartupHandlerLinkDef.h
  OPTIONS 
    -I${FFTW3_INCLUDE_DIR} -I${MUSRFIT_INC}
    -inlineInputHeader
  MODULE PStartupHandler
)
root_generate_dictionary(
  PUserFcnBaseDict
  PUserFcnBase.h  
  LINKDEF ${MUSRFIT_INC}/PUserFcnBaseLinkDef.h
  OPTIONS
    -inlineInputHeader
    -I${FFTW3_INCLUDE_DIR} -I${MUSRFIT_INC}
  MODULE PUserFcnBase
)
root_generate_dictionary(
  PRgeHandlerDict 
  PRgeHandler.h 
  OPTIONS 
    -I${NONLOCAL_INC} 
    -I${MUSRFIT_INC}
    -I${CMAKE_CURRENT_SOURCE_DIR}
    -inlineInputHeader
  LINKDEF ${MUSRFIT_INC}/PRgeHandlerLinkDef.h
  MODULE PRgeHandler
)


#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(MUSR_VERSION "1.3.0")
set(MUSR_LIBRARY_NAME "PMusr")
configure_file("PMusr.pc.in" "PMusr.pc" @ONLY)
set(USERFCN_LIBRARY_NAME "PUserFcnBase")
configure_file("PUserFcnBase.pc.in" "PUserFcnBase.pc" @ONLY)
set(PRGEHANDLER_LIBRARY_NAME "PRgeHandler")
configure_file("PRgeHandler.pc.in" "PRgeHandler.pc" @ONLY)

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
  PRunAsymmetryBNMR.cpp
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
#--- make sure that the include directory is found ----------------------------
target_include_directories(
  PMusr BEFORE PRIVATE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}> 
    $<BUILD_INTERFACE:${NEXUS_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/external/MusrRoot>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/external/TLemRunHeader>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/external/MuSR_software/Class_MuSR_PSI>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/external/mud/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/external/nexus>
)

add_library(PUserFcnBase SHARED
  PUserFcnBase.cpp
  PUserFcnBaseDict.cxx
)

add_library(PRgeHandler SHARED
  PRgeHandler.cpp
  PRgeHandlerDict.cxx
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(PMusr
  PROPERTIES
    VERSION ${MUSR_VERSION}
)
set_target_properties(PUserFcnBase
  PROPERTIES
    VERSION ${MUSR_VERSION}
)
set_target_properties(PRgeHandler
  PROPERTIES
    VERSION ${MUSR_VERSION}
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  PUserFcnBase BEFORE PRIVATE $<BUILD_INTERFACE:${MUSRFIT_INC}>
)

target_include_directories(
  PRgeHandler BEFORE PRIVATE $<BUILD_INTERFACE:${MUSRFIT_INC}>
)

#--- add OpenMP compile options if needed -------------------------------------
if (OpenMP_FOUND)
  target_compile_options(PMusr PUBLIC ${OpenMP_CXX_FLAGS})
endif (OpenMP_FOUND)

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
if (OpenMP_FOUND)
  if (OpenMP_CXX_LIBRARIES)
    set(DependOnLibs ${DependOnLibs} ${OpenMP_CXX_LIBRARIES})
  else (OpenMP_CXX_LIBRARIES)
    set(DependOnLibs ${DependOnLibs} ${OpenMP_CXX_FLAGS}) # for older cmake OpenMP_CXX_LIBRARIES is not defined
  endif (OpenMP_CXX_LIBRARIES)
endif (OpenMP_FOUND)

target_link_libraries(PUserFcnBase ${ROOT_LIBRARIES})
target_link_libraries(PRgeHandler ${Boost_LIBRARIES} ${ROOT_LIBRARIES})
target_link_libraries(PMusr ${DependOnLibs})

#--- install PUserFcnBase solib -----------------------------------------------
install(TARGETS PUserFcnBase DESTINATION lib)

#--- install PRgeHandler solib -----------------------------------------------
install(TARGETS PRgeHandler DESTINATION lib)

#--- install PMusr solib ------------------------------------------------------
install(TARGETS PMusr DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libPFourierCanvas_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libPFourierCanvas.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libPMusrCanvas_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libPMusrCanvas.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libPMusrT0_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libPMusrT0.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libPRgeHandler_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libPRgeHandler.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libPStartupHandler_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libPStartupHandler.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libPUserFcnBase_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libPUserFcnBase.rootmap
  DESTINATION lib
)

#--- install headers ----------------------------------------------------------
install(
  FILES ${MUSRFIT_INC}/PFitterFcn.h
        ${MUSRFIT_INC}/PFitter.h
        ${MUSRFIT_INC}/PFourierCanvas.h
        ${MUSRFIT_INC}/PFourier.h 
        ${MUSRFIT_INC}/PFunctionGrammar.h
        ${MUSRFIT_INC}/PFunction.h
        ${MUSRFIT_INC}/PFunctionHandler.h
        ${MUSRFIT_INC}/PMsr2Data.h
        ${MUSRFIT_INC}/PMsrHandler.h
        ${MUSRFIT_INC}/PMusrCanvas.h
        ${MUSRFIT_INC}/PMusr.h
        ${MUSRFIT_INC}/PMusrT0.h
        ${MUSRFIT_INC}/PPrepFourier.h
        ${MUSRFIT_INC}/PRgeHandler.h
        ${MUSRFIT_INC}/PRunAsymmetry.h
        ${MUSRFIT_INC}/PRunAsymmetryBNMR.h
        ${MUSRFIT_INC}/PRunAsymmetryRRF.h
        ${MUSRFIT_INC}/PRunBase.h
        ${MUSRFIT_INC}/PRunDataHandler.h
        ${MUSRFIT_INC}/PRunListCollection.h
        ${MUSRFIT_INC}/PRunMuMinus.h
        ${MUSRFIT_INC}/PRunNonMusr.h
        ${MUSRFIT_INC}/PRunSingleHisto.h
        ${MUSRFIT_INC}/PRunSingleHistoRRF.h
        ${MUSRFIT_INC}/PStartupHandler.h
        ${MUSRFIT_INC}/PTheory.h
        ${MUSRFIT_INC}/PUserFcnBase.h
  DESTINATION include
)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/PUserFcnBase.pc
        ${CMAKE_CURRENT_BINARY_DIR}/PRgeHandler.pc
        ${CMAKE_CURRENT_BINARY_DIR}/PMusr.pc
  DESTINATION lib/pkgconfig
)

