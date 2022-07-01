#--- add further sub-directories ----------------------------------------------
add_subdirectory(classes)
add_subdirectory(external)
if (Qt6Core_FOUND)
  add_subdirectory(musredit_qt6)
elseif (Qt5Core_FOUND)
  add_subdirectory(musredit_qt5)
elseif (Qt4_FOUND)
  add_subdirectory(musredit)
elseif (QT_FOUND)
  add_subdirectory(musrgui)
endif ()

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
if (IS_GIT_REPO)
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
  set(HAVE_GIT_REV_H "-DHAVE_GIT_REV_H")  
  set(GIT_REV_H "git-revision.h")
else (IS_GIT_REPO)
  set(HAVE_GIT_REV_H "")  
  set(GIT_REV_H "")
endif (IS_GIT_REPO)

#--- end create git-revision.h ------------------------------------------------

#--- add all executables ------------------------------------------------------
add_executable(addRun ${GIT_REV_H} addRun.cpp)
target_compile_options(addRun BEFORE PRIVATE "-DHAVE_CONFIG_H" "${HAVE_GIT_REV_H}")
target_include_directories(addRun 
  BEFORE PRIVATE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
)
target_link_libraries(addRun ${ROOT_LIBRARIES} ${MUSRFIT_LIBS} ${Boost_LIBRARIES})

add_executable(any2many ${GIT_REV_H} any2many.cpp)
target_compile_options(any2many BEFORE PRIVATE "-DHAVE_CONFIG_H" "${HAVE_GIT_REV_H}")
target_include_directories(any2many 
  BEFORE PRIVATE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
)
target_link_libraries(any2many ${ROOT_LIBRARIES} ${MUSRFIT_LIBS})

add_executable(dump_header ${GIT_REV_H} dump_header.cpp)
target_compile_options(dump_header BEFORE PRIVATE "-DHAVE_CONFIG_H" "${HAVE_GIT_REV_H}")
target_include_directories(dump_header 
  BEFORE PRIVATE 
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
    $<BUILD_INTERFACE:${NEXUS_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/external/MusrRoot>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/external/TLemRunHeader>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/external/MuSR_software/Class_MuSR_PSI/>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/external/mud/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/external/nexus>
)
target_link_libraries(dump_header ${ROOT_LIBRARIES} ${MUSRFIT_LIBS})

add_executable(msr2data ${GIT_REV_H} msr2data.cpp)
target_compile_options(msr2data BEFORE PRIVATE "-DHAVE_CONFIG_H" "${HAVE_GIT_REV_H}")
target_include_directories(msr2data 
  BEFORE PRIVATE 
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
)
target_link_libraries(msr2data ${ROOT_LIBRARIES} ${MUSRFIT_LIBS})

add_executable(msr2msr msr2msr.cpp)
target_link_libraries(msr2msr ${ROOT_LIBRARIES})

add_executable(musrfit ${GIT_REV_H} musrfit.cpp)
target_compile_options(musrfit BEFORE PRIVATE "-DHAVE_CONFIG_H" "${HAVE_GIT_REV_H}")
target_include_directories(musrfit 
  BEFORE PRIVATE 
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
)
target_link_libraries(musrfit ${ROOT_LIBRARIES} ${MUSRFIT_LIBS})

add_executable(musrFT ${GIT_REV_H} musrFT.cpp)
target_compile_options(musrFT BEFORE PRIVATE "-DHAVE_CONFIG_H" "${HAVE_GIT_REV_H}")
target_include_directories(musrFT 
  BEFORE PRIVATE 
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${FFTW3_INCLUDE}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
)
target_link_libraries(musrFT  FFTW3::FFTW3 ${ROOT_LIBRARIES} ${MUSRFIT_LIBS})

add_executable(musrRootValidation ${GIT_REV_H} musrRootValidation.cpp)
target_compile_options(musrRootValidation BEFORE PRIVATE "-DHAVE_CONFIG_H" "${HAVE_GIT_REV_H}")
target_include_directories(musrRootValidation 
  BEFORE PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
    $<BUILD_INTERFACE:${LIBXML2_INCLUDE_DIR}>
)
target_link_libraries(musrRootValidation ${ROOT_LIBRARIES} ${MUSRFIT_LIBS} ${LIBXML2_LIBRARIES})

add_executable(musrt0 ${GIT_REV_H} musrt0.cpp)
target_compile_options(musrt0 BEFORE PRIVATE "-DHAVE_CONFIG_H" "${HAVE_GIT_REV_H}")
target_include_directories(musrt0 
  BEFORE PRIVATE 
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
)
target_link_libraries(musrt0 ${ROOT_LIBRARIES} ${MUSRFIT_LIBS})

add_executable(musrview ${GIT_REV_H} musrview.cpp)
target_compile_options(musrview BEFORE PRIVATE "-DHAVE_CONFIG_H" "${HAVE_GIT_REV_H}")
target_include_directories(musrview 
  BEFORE PRIVATE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${FFTW3_INCLUDE}> 
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
)
target_link_libraries(musrview FFTW3::FFTW3 ${ROOT_LIBRARIES} ${MUSRFIT_LIBS})

add_executable(write_musrRoot_runHeader ${GIT_REV_H} write_musrRoot_runHeader.cpp)
target_compile_options(write_musrRoot_runHeader BEFORE PRIVATE "-DHAVE_CONFIG_H" "${HAVE_GIT_REV_H}")
target_include_directories(write_musrRoot_runHeader 
  BEFORE PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
    $<BUILD_INTERFACE:${LIBXML2_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/external/MusrRoot>
)
target_link_libraries(write_musrRoot_runHeader ${ROOT_LIBRARIES} ${MUSRFIT_LIBS} ${LIBXML2_LIBRARIES})

#--- installation info --------------------------------------------------------
install(
  TARGETS
    addRun
    any2many
    dump_header
    msr2data
    msr2msr
    musrfit
    musrFT
    musrRootValidation
    musrt0
    musrview
    write_musrRoot_runHeader
  RUNTIME DESTINATION 
    bin
)


