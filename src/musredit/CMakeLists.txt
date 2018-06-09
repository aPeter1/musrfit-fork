#--- musredit for Qt > 4.6 and < 5.0 ------------------------------------------

#--- create musrfit-info.h ----------------------------------------------------
configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/musrfit-info.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/musrfit-info.h
)

#--- sources, headers, ... ----------------------------------------------------
set(musredit_src
  main.cpp
  PAdmin.cpp
  PDumpOutputHandler.cpp
  PFindDialog.cpp
  PFitOutputHandler.cpp
  PGetAsymmetryRunBlockDialog.cpp
  PGetDefaultDialog.cpp
  PGetFourierBlockDialog.cpp
  PGetFunctionsBlockDialog.cpp
  PGetMusrFTOptionsDialog.cpp
  PGetNonMusrRunBlockDialog.cpp
  PGetParameterBlockDialog.cpp
  PGetPlotBlockDialog.cpp
  PGetSingleHistoRunBlockDialog.cpp
  PGetTheoryBlockDialog.cpp
  PGetTitleBlockDialog.cpp
  PHelp.cpp
  PMsr2DataDialog.cpp
  PMusrEditAbout.cpp
  PPrefsDialog.cpp
  PReplaceConfirmationDialog.cpp
  PReplaceDialog.cpp
  PSubTextEdit.cpp
  PTextEdit.cpp
)

set(musredit_h
  musredit.h
  PAdmin.h
  PDumpOutputHandler.h
  PFindDialog.h
  PFitOutputHandler.h
  PGetAsymmetryRunBlockDialog.h
  PGetDefaultDialog.h
  PGetFourierBlockDialog.h
  PGetFunctionsBlockDialog.h
  PGetMusrFTOptionsDialog.h
  PGetNonMusrRunBlockDialog.h
  PGetParameterBlockDialog.h
  PGetPlotBlockDialog.h
  PGetSingleHistoRunBlockDialog.h
  PGetTheoryBlockDialog.h
  PGetTitleBlockDialog.h
  PHelp.h
  PMsr2DataDialog.h
  PMusrEditAbout.h
  PPrefsDialog.h
  PReplaceConfirmationDialog.h
  PReplaceDialog.h
  PSubTextEdit.h
  PTextEdit.h
)

set(musredit_ui
  forms/PFindDialog.ui
  forms/PGetAsymmetryRunBlockDialog.ui
  forms/PGetDefaultDialog.ui
  forms/PGetFourierBlockDialog.ui
  forms/PGetFunctionsBlockDialog.ui
  forms/PGetMusrFTOptionsDialog.ui
  forms/PGetNonMusrRunBlockDialog.ui
  forms/PGetParameterBlockDialog.ui
  forms/PGetPlotBlockDialog.ui
  forms/PGetSingleHistoRunBlockDialog.ui
  forms/PGetTheoryBlockDialog.ui
  forms/PGetTitleBlockDialog.ui
  forms/PMsr2DataDialog.ui
  forms/PMusrEditAbout.ui
  forms/PPrefsDialog.ui
  forms/PReplaceConfirmationDialog.ui
  forms/PReplaceDialog.ui
)

set(musredit_rcc
  musredit.qrc
)

# Next, using precompiler, compiler and linker
include(${QT_USE_FILE})

# using Qt meta-system (precompiler)
QT4_ADD_RESOURCES(RESOURCES ${musredit_rcc})
QT4_WRAP_UI( UI_HEADERS ${musredit_ui})
QT4_WRAP_CPP( MOC_SRCS ${musredit_h})

# define target
add_executable(musredit ${musredit_src} ${MOC_SRCS} ${RESOURCES} ${UI_HEADERS})

target_include_directories(musredit
  BEFORE PRIVATE
  $<BUILD_INTERFACE:${QT_INCLUDES}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
)

target_compile_definitions(musredit
  PRIVATE
  ${QT_DEFINITIONS}
)

target_link_libraries(musredit 
  Qt4::QtCore
  Qt4::QtGui
  Qt4::QtNetwork
  Qt4::QtWebKit
  Qt4::QtXml
)

#--- installation info --------------------------------------------------------
install(
  TARGETS
    musredit
  RUNTIME DESTINATION
    bin
)

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


