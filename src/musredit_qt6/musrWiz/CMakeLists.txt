#--- musrWiz for Qt > 6.0 -----------------------------------------------------

set(qt_libs Qt6::Core Qt6::Widgets Qt6::Svg Qt6::Xml)

set(musrWiz_src
  PTheoTemplate.cpp
  PMusrfitFunc.cpp
  PInstrumentDef.cpp
  PAdmin.cpp
  PMusrWiz.cpp
  musrWiz.cpp
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
qt6_add_resources(musrWiz_rcc musrWiz.qrc)

# remove generated files from automoc and autouic
set_property(SOURCE qrc_musrWiz.cpp PROPERTY SKIP_AUTOMOC ON)

set(macosx_icon icons/musrWiz.icns)
if (APPLE)
  add_executable(musrWiz MACOSX_BUNDLE
    ${musrWiz_src}
    ${musrWiz_rcc}
    ${macosx_icon}    
  )
else (APPLE)
  add_executable(musrWiz 
    ${musrWiz_src}
    ${musrWiz_rcc}
  )
endif (APPLE)

target_include_directories(musrWiz
  BEFORE PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
)

target_link_libraries(musrWiz ${qt_libs})

#--- installation info --------------------------------------------------------
if (APPLE)
  set_target_properties(musrWiz PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "musrWiz"
    MACOSX_BUNDLE_INFO_STRING "musrfit: musrWiz allows to create input msr-files if no templates are available."
    MACOSX_BUNDLE_ICON_FILE "musrWiz.icns"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "ch.psi.lmu.musrWiz"
    MACOSX_BUNDLE_COPYRIGHT "Andreas Suter"
    RESOURCE ${macosx_icon}
  )
endif (APPLE)

if (APPLE)
  install( TARGETS musrWiz
    BUNDLE DESTINATION /Applications
  )
else (APPLE)
  install( TARGETS musrWiz
    RUNTIME DESTINATION bin
  )
endif (APPLE)
