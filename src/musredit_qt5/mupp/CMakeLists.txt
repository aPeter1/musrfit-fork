#--- mupp for Qt > 5.0 --------------------------------------------------------

#--- Find includes in corresponding build directories -------------------------
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#--- instruct CMake to run moc automatically when needed ----------------------
set(CMAKE_AUTOMOC ON)

#--- define mupp version ------------------------------------------------------
set(mupp_VERSION 1.0.0)

#--- mupp_version.h generation - START ----------------------------------------
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_mupp_version_file.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/configure_mupp_version_file.cmake
  @ONLY
)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/mupp_version.h
  COMMAND ${CMAKE_COMMAND} -P
    ${CMAKE_CURRENT_BINARY_DIR}/configure_mupp_version_file.cmake
  DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/configure_mupp_version_file.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/mupp_version.h.in
  COMMENT "Configuring mupp_version.h"
  VERBATIM
)

add_custom_target(
  configure_mupp_version ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/mupp_version.h
)
#--- mupp_version.h generation - END ------------------------------------------

add_subdirectory(plotter)

qt5_add_resources(qrc_mupp.cpp mupp.qrc)
set_property(SOURCE qrc_mupp.cpp PROPERTY SKIP_AUTOMOC ON) # needed for cmake 3.x

set(GENERATED_HEADER_FILES
  mupp_version.h
)
set_property(SOURCE mupp_version.h PROPERTY SKIP_AUTOMOC ON) # needed for cmake 3.x

set(MUPP_SOURCE_FILES
  mupp.cpp 
  PmuppAdmin.cpp 
  Pmupp.cpp
  PmuppScript.cpp 
  PmuppGui.cpp
  PVarDialog.cpp
)

if (APPLE)
  set(RESOURCE_FILES icons/mupp.icns)
  add_executable(mupp 
     MACOSX_BUNDLE ${GENERATED_HEADER_FILES} ${MUPP_SOURCE_FILES} 
     qrc_mupp.cpp ${RESOURCE_FILES}
  )
else (APPLE)
  add_executable(mupp ${GENERATED_HEADER_FILES} ${MUPP_SOURCE_FILES} qrc_mupp.cpp)
endif (APPLE)

#--- add the variable related sources -----------------------------------------
add_subdirectory(var)

#--- add the necessary header includes ----------------------------------------
target_include_directories(mupp 
  BEFORE PRIVATE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/var/include>
)

#--- use the Widgets and XML modules from Qt5 ---------------------------------
target_link_libraries(mupp Qt5::Widgets Qt5::Xml)

#--- if macOS make an app rather than just a command line executable ----------
set_target_properties(mupp PROPERTIES
  VERSION ${mupp_VERSION}
)
if (APPLE)
  set_target_properties(mupp PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "mupp"
    MACOSX_BUNDLE_INFO_STRING "mupp is used to plot parameters from musrfit collections."
    MACOSX_BUNDLE_ICON_FILE "mupp.icns"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${mupp_VERSION}"
    MACOSX_FRAMEWORK_IDENTIFIER ch.psi.mupp
    MACOSX_BUNDLE_COPYRIGHT "Andreas Suter"
    RESOURCE "${RESOURCE_FILES}"
  )
endif (APPLE)

#--- install ------------------------------------------------------------------
if (APPLE)
  install(TARGETS mupp BUNDLE DESTINATION /Applications)
else (APPLE)
  install(TARGETS mupp DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif (APPLE)
