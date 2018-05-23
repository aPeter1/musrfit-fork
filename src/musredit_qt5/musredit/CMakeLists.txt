#--- musredit for Qt > 5.0 ----------------------------------------------------

message("debug> will deal with the build of musredit (Qt 5)")

#--- check if Qt5WebEngine or Qt5WebKit is present ----------------------------
find_package(Qt5WebEngine QUIET)
find_package(Qt5WebKit QUIET)

set(qt_libs Qt5::Core Qt5::Widgets Qt5::Network Qt5::Xml Qt5::Svg Qt5::PrintSupport)
if (Qt5WebEngine_FOUND)
  message("debug> Qt5WebEngine is present.")
  set(qt_libs ${qt_libs} Qt5::WebEngine)
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

add_executable(musredit 
  ${musredit_src}
  ${out_ui}
  ${musredit_rcc}
)

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
