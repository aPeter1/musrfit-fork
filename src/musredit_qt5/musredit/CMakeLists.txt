#--- musredit for Qt > 5.0 ----------------------------------------------------

#--- check if Qt5WebEngine or Qt5WebKit is present ----------------------------
find_package(Qt5WebEngine QUIET)
find_package(Qt5WebKit QUIET)

set(qt_libs Qt5::Core Qt5::Widgets Qt5::Network Qt5::Xml Qt5::Svg Qt5::PrintSupport)
if (Qt5WebEngine_FOUND)
  message("debug> Qt5WebEngine is present.")
  find_package(Qt5WebEngineWidgets QUIET CONFIG REQUIRED)
  set(qt_libs ${qt_libs} Qt5::WebEngine Qt5::WebEngineWidgets)
elseif (Qt5WebKit_FOUND)
  message("debug> Qt5WebKit is present.")
  find_package(Qt5WebKitWidgets QUIET CONFIG REQUIRED)
  set(qt_libs ${qt_libs} Qt5::WebKit Qt5::WebKitWidgets)
else (Qt5WebEngine_FOUND)
  message("debug> Neither Qt5WebEngine nor Qt5WebKit found.")
endif (Qt5WebEngine_FOUND)

set(musredit_src
  PHelp.cpp
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

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

#[==[ 
# as35 currently CMAKE_AUTOUIC -> ON doesn't work since it requires the ui-files
#      in the same directory as the cpp-files.
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
#]==]
set(CMAKE_AUTOUIC OFF)

# call qt/uic
qt5_wrap_ui(out_ui ${musredit_ui})
# add qt/rcc
qt5_add_resources(musredit_rcc musredit.qrc)

set(macosx_icon icons/musredit.icns)
if (APPLE)
  add_executable(musredit MACOSX_BUNDLE
    ${musredit_src}
    ${out_ui}
    ${musredit_rcc}
    ${macosx_icon}
  )
else (APPLE)
  add_executable(musredit 
    ${musredit_src}
    ${out_ui}
    ${musredit_rcc}
  )
endif (APPLE)

if (Qt5WebEngine_FOUND)
  target_compile_options(musredit
    BEFORE PRIVATE 
    -DHAVE_QT_WEB_ENGINE
  )
endif (Qt5WebEngine_FOUND)

target_include_directories(musredit
  BEFORE PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
)

target_link_libraries(musredit ${qt_libs})

#--- installation info --------------------------------------------------------
if (APPLE)
  set_target_properties(musredit PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "musredit"
    MACOSX_BUNDLE_INFO_STRING "musrfit: musredit simplifies the handling of the msr-files for uSR fitting."
    MACOSX_BUNDLE_ICON_FILE "musredit.icns"
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

