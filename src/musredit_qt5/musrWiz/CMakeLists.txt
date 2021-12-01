#--- musrWiz for Qt = 5.x -----------------------------------------------------

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

set(qt_libs Qt5::Core Qt5::Widgets Qt5::Svg Qt5::Xml)

set(musrWiz_src
  PTheoTemplate.cpp
  PMusrfitFunc.cpp
  PInstrumentDef.cpp
  PAdmin.cpp
  PMusrWiz.cpp
  musrWiz.cpp
)

set(macosx_icon icons/musrWiz.icns)
if (APPLE)
  add_executable(musrWiz MACOSX_BUNDLE
    ${musrWiz_src}
    musrWiz.qrc
    ${macosx_icon}    
  )
else (APPLE)
  add_executable(musrWiz 
    ${musrWiz_src}
    musrWiz.qrc
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

target_compile_options(musrWiz
  PRIVATE
    "${HAVE_GIT_REV_H}"  
)

target_link_libraries(musrWiz PRIVATE ${qt_libs})

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
