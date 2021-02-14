#--- musredit for Qt > 5.0 ----------------------------------------------------

#--- check if Qt5WebEngine or Qt5WebKit is present ----------------------------
#find_package(Qt5WebEngine QUIET)
#find_package(Qt5WebKit QUIET)

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

# remove generated files from automoc and autouic
set_property(SOURCE ui_PFindDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PReplaceDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PReplaceConfirmationDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PMusrEditAbout.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PPrefsDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PGetMusrFTOptionsDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PGetTitleBlockDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PGetParameterBlockDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PGetTheoryBlockDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PGetFunctionsBlockDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PGetAsymmetryRunBlockDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PGetSingleHistoRunBlockDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PGetNonMusrRunBlockDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PGetFourierBlockDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PGetPlotBlockDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PMsr2DataDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ui_PChangeDefaultPathsDialog.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE qrc_musredit.cpp PROPERTY SKIP_AUTOMOC ON)

if (${CMAKE_HOST_SYSTEM_VERSION} GREATER_EQUAL "20.3.0")
  set(macosx_icon icons/musredit-bigsur.icns)
else()	
  set(macosx_icon icons/musredit.icns)
endif()
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

# set necessary tags depending if QtWebEngine, QtWebKit, 
# or none of both are given
if (Qt5WebKit_FOUND)
  target_compile_options(musredit
    BEFORE PRIVATE 
    -DHAVE_QT_WEB_KIT
  )
endif (Qt5WebKit_FOUND)

if (Qt5WebEngine_FOUND)
  target_compile_options(musredit
    BEFORE PRIVATE 
    -DHAVE_QT_WEB_ENGINE
  )
endif (Qt5WebEngine_FOUND)

if (Qt5NoWeb)
  target_compile_options(musredit
    BEFORE PRIVATE 
    -DHAVE_QT_NO_WEB
  )
endif (Qt5NoWeb)

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
    if (${CMAKE_HOST_SYSTEM_VERSION} GREATER_EQUAL "20.3.0")
      MACOSX_BUNDLE_ICON_FILE "musredit-bigsur.icns"
    else()
      MACOSX_BUNDLE_ICON_FILE "musredit.icns"
    endif()  
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

