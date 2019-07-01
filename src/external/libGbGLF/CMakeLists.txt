# GbGLF library ---------------------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)

# ROOT requires that the dictonary header files are found at configuration time.
# Hence, target_include_directories cannot be used here because, targets are 
# setup only afterwards.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

root_generate_dictionary(
  PGbGLFDict 
  PGbGLF.h 
  OPTIONS
    -I${MUSRFIT_INC}
    -I${CMAKE_CURRENT_SOURCE_DIR}
    -inlineInputHeader
  LINKDEF PGbGLFLinkDef.h
  MODULE PGbGLF
)

#--- create pkg-config info ---------------------------------------------------
set(PGBGLF_VERSION "1.0.0")
# nothing more for now

#--- lib creation -------------------------------------------------------------
add_library(PGbGLF SHARED
  PGbGLF.cpp
  PGbGLFDict.cxx
)
#--- make sure that the include directory is found ----------------------------
target_include_directories(
  PGbGLF BEFORE PRIVATE 
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}> 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(PGbGLF
  PROPERTIES
    VERSION ${PGBGLF_VERSION}
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(PGbGLF ${FFTW3_LIBRARY} ${ROOT_LIBRARIES} PUserFcnBase)

#--- install PGbGLF solib -----------------------------------------------------
install(TARGETS PGbGLF DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libPGbGLF_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libPGbGLF.rootmap
  DESTINATION lib
)

#--- install PGbGLF header ----------------------------------------------------
install(
  FILES
    PGbGLF.h 
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
# nothing here for now
