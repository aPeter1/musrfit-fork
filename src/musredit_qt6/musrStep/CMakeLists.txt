#--- musrStep for Qt > 6.0 ----------------------------------------------------

set(qt_libs Qt6::Core Qt6::Widgets Qt6::Svg)

set(musrStep_src
  PMusrStep.cpp
  musrStep.cpp
)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

#[==[ 
# as35 currently CMAKE_AUTOUIC -> ON doesn't work since it requires the ui-files
#      in the same directory as the cpp-files.
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
#]==]
set(CMAKE_AUTOUIC OFF)

# add qt/rcc
qt6_add_resources(musrStep_rcc musrStep.qrc)

# remove generated files from automoc and autouic
set_property(SOURCE qrc_musrStep.cpp PROPERTY SKIP_AUTOMOC ON)

set(macosx_icon icons/musrStep.icns)
if (APPLE)
  add_executable(musrStep MACOSX_BUNDLE
    ${musrStep_src}
    ${musrStep_rcc}
    ${macosx_icon}
  )
else (APPLE)
  add_executable(musrStep 
    ${musrStep_src}
    ${musrStep_rcc}
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

target_link_libraries(musrStep ${qt_libs})

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
