#--- musredit for Qt = 5.x ----------------------------------------------------

#--- check if project source is repo ------------------------------------------
if (IS_GIT_REPO)
  set(HAVE_GIT_REV_H "-DHAVE_GIT_REV_H")
else (IS_GIT_REPO)
  set(HAVE_GIT_REV_H "")
endif (IS_GIT_REPO)

#--- instruct CMake to run moc automatically when needed ----------------------
set(CMAKE_AUTOMOC ON)
#--- create code from a list of Qt designer ui files --------------------------
set(CMAKE_AUTOUIC ON)
#--- add forms to the uic search path -----------------------------------------
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms)
#--- deal with the resources --------------------------------------------------
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(qt_libs Qt5::Core Qt5::Widgets Qt5::Network Qt5::Xml Qt5::Svg Qt5::PrintSupport)

set(musredit_src
  PTextEdit.cpp
  PSubTextEdit.cpp
  PAdmin.cpp
  PFindDialog.cpp
  PReplaceDialog.cpp
  PReplaceConfirmationDialog.cpp
  PFitOutputHandler.cpp
  PDumpOutputHandler.cpp
  PPrefsDialog.cpp
  PGetMusrFTOptionsDialog.cpp
  PGetTitleBlockDialog.cpp
  PGetParameterBlockDialog.cpp
  PGetTheoryBlockDialog.cpp
  PGetFunctionsBlockDialog.cpp
  PGetAsymmetryRunBlockDialog.cpp
  PGetSingleHistoRunBlockDialog.cpp
  PGetNonMusrRunBlockDialog.cpp
  PGetFourierBlockDialog.cpp
  PGetPlotBlockDialog.cpp
  PMsr2DataDialog.cpp
  PChangeDefaultPathsDialog.cpp
  PMusrEditAbout.cpp
  main.cpp
)

set(musredit_ui
  forms/PFindDialog.ui
  forms/PReplaceDialog.ui
  forms/PReplaceConfirmationDialog.ui
  forms/PMusrEditAbout.ui
  forms/PPrefsDialog.ui
  forms/PGetMusrFTOptionsDialog.ui
  forms/PGetTitleBlockDialog.ui
  forms/PGetParameterBlockDialog.ui
  forms/PGetTheoryBlockDialog.ui
  forms/PGetFunctionsBlockDialog.ui
  forms/PGetAsymmetryRunBlockDialog.ui
  forms/PGetSingleHistoRunBlockDialog.ui
  forms/PGetNonMusrRunBlockDialog.ui
  forms/PGetFourierBlockDialog.ui
  forms/PGetPlotBlockDialog.ui
  forms/PMsr2DataDialog.ui
  forms/PChangeDefaultPathsDialog.ui
)

if (APPLE)
  if (${CMAKE_HOST_SYSTEM_VERSION} GREATER_EQUAL "20.3.0")
    set(macosx_icon_name musredit-bigsur.icns)
  else()
    set(macosx_icon_name musredit.icns)
  endif()
  set(macosx_icon "icons/${macosx_icon_name}")
message(STATUS "macosx_icon: ${macosx_icon}")
  add_executable(musredit MACOSX_BUNDLE
    ${musredit_src}
    ${musredit_ui}
    musredit.qrc
    ${macosx_icon}
  )
else (APPLE)
  add_executable(musredit
    ${musredit_src}
    ${musredit_ui}
    musredit.qrc
  )
endif (APPLE)

target_include_directories(musredit
  BEFORE PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../../..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
)

target_compile_options(musredit
  PRIVATE
    "${HAVE_GIT_REV_H}"
)

target_link_libraries(musredit PRIVATE ${qt_libs})

#--- installation info --------------------------------------------------------
if (APPLE)
  set_target_properties(musredit PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "musredit"
    MACOSX_BUNDLE_INFO_STRING "musrfit: musredit simplifies the handling of the msr-files for uSR fitting."
    MACOSX_BUNDLE_ICON_FILE "${macosx_icon_name}"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "ch.psi.lmu.musredit"
    MACOSX_BUNDLE_COPYRIGHT "Andreas Suter"
    RESOURCE ${macosx_icon}
  )
endif (APPLE)

if (APPLE)
  install( TARGETS musredit
    BUNDLE DESTINATION /Applications
  )
else (APPLE)
  install( TARGETS musredit
    RUNTIME DESTINATION bin
  )
endif (APPLE)

#--- documentation installation info ------------------------------------------
install(
  DIRECTORY
    ${CMAKE_SOURCE_DIR}/doc/examples
    ${CMAKE_SOURCE_DIR}/doc/html
    ${CMAKE_SOURCE_DIR}/doc/memos
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/share/doc/musrfit
  MESSAGE_NEVER
)
