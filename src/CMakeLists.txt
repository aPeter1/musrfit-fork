#--- add further sub-directories ----------------------------------------------
add_subdirectory(classes)
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
set(MUSRFIT_LIBS ${MUSRFIT_LIBS} mud)
set(MUSRFIT_LIBS ${MUSRFIT_LIBS} TMusrRunHeader)
set(MUSRFIT_LIBS ${MUSRFIT_LIBS} TLemRunHeader)
set(MUSRFIT_LIBS ${MUSRFIT_LIBS} Class_MuSR_PSI)
if (nexus)
  set(MUSRFIT_LIBS ${MUSRFIT_LIBS} ${NEXUS_LIBRARY})
  set(MUSRFIT_LIBS ${MUSRFIT_LIBS} PNeXus)
endif (nexus)
set(MUSRFIT_LIBS ${MUSRFIT_LIBS} PMusr)

#--- start create git-revision.h ----------------------------------------------
configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/configure_musrfit_version_file.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/configure_musrfit_version_file.cmake
  @ONLY
)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/git-revision.h
  COMMAND ${CMAKE_COMMAND} -P
    ${CMAKE_CURRENT_BINARY_DIR}/configure_musrfit_version_file.cmake
  DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/configure_musrfit_version_file.cmake
    ${CMAKE_SOURCE_DIR}/cmake/git-revision.h.in
  COMMENT "Configuring git-revision.h"
  VERBATIM
)

add_custom_target(
  configure_musrfit_version ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/git-revision.h
)
#--- end create git-revision.h ------------------------------------------------

#--- add all executables ------------------------------------------------------
message("debug> ${CMAKE_BINARY_DIR}")
add_executable(any2many git-revision.h any2many.cpp)
target_include_directories(any2many BEFORE PRIVATE $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>)
target_link_libraries(any2many ${ROOT_LIBRARIES} ${FFTW3_LIBRARY} ${MUSRFIT_LIBS})

