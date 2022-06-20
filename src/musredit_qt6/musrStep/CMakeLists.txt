#--- musrStep for Qt = 6.x ----------------------------------------------------

#--- check if project source is repo ------------------------------------------
if (IS_GIT_REPO)
  set(HAVE_GIT_REV_H "-DHAVE_GIT_REV_H")
else (IS_GIT_REPO)
  set(HAVE_GIT_REV_H "")
endif (IS_GIT_REPO)

#--- instruct CMake to run moc automatically when needed ----------------------
set(CMAKE_AUTOMOC ON)
#--- deal with the resources --------------------------------------------------
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(qt_libs Qt6::Core Qt6::Widgets Qt6::Svg)

set(musrStep_src
  PMusrStep.cpp
  musrStep.cpp
)

set(macosx_icon icons/musrStep.icns)
if (APPLE)
  add_executable(musrStep MACOSX_BUNDLE
    ${musrStep_src}
    musrStep.qrc
    ${macosx_icon}
  )
else (APPLE)
  add_executable(musrStep
    ${musrStep_src}
    musrStep.qrc
  )
endif (APPLE)

target_include_directories(musrStep
  BEFORE PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
)

target_compile_options(musrStep
  PRIVATE
    "${HAVE_GIT_REV_H}"
)

target_link_libraries(musrStep PRIVATE ${qt_libs})

#--- installation info --------------------------------------------------------
if (APPLE)
  set_target_properties(musrStep PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "musrStep"
    MACOSX_BUNDLE_INFO_STRING "musrfit: musrStep allows to reset/set the initial step size of a msr-files."
    MACOSX_BUNDLE_ICON_FILE "musrStep.icns"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "ch.psi.lmu.musrStep"
    MACOSX_BUNDLE_COPYRIGHT "Andreas Suter"
    RESOURCE ${macosx_icon}
    INSTALL_RPATH "${Qt6_DIR}/../.."
  )
endif (APPLE)

if (APPLE)
  install( TARGETS musrStep
    BUNDLE DESTINATION /Applications
  )
else (APPLE)
  install( TARGETS musrStep
    RUNTIME DESTINATION bin
  )
endif (APPLE)
