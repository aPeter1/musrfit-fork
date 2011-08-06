TEMPLATE = app
TARGET = musredit

!exists( musredit_startup.xml ) {
    error( "Configuration file musredit_startup.xml not found! Please configure musrfit first, before trying to install musredit!" )
}

# install path for musredit
count( PREFIX, 1 ) {
    MUSREDIT_INSTALL_PATH = $${PREFIX}/bin
}
isEmpty( MUSREDIT_INSTALL_PATH ) {
    MUSR_FIT_PATH = $$(MUSRFITPATH)
    count( MUSR_FIT_PATH, 1 ) {
        MUSREDIT_INSTALL_PATH = $$(MUSRFITPATH)
    }
}
isEmpty( MUSREDIT_INSTALL_PATH ) {
    ROOT_SYS_PATH = $$(ROOTSYS)
    count( ROOT_SYS_PATH, 1 ) {
        MUSREDIT_INSTALL_PATH = $$(ROOTSYS)/bin
    }
}
isEmpty( MUSREDIT_INSTALL_PATH ) {
    MUSREDIT_INSTALL_PATH = /usr/local/bin
}

unix {
    message( "Determined installation path: $${MUSREDIT_INSTALL_PATH}" )
}

unix:target.path = $${MUSREDIT_INSTALL_PATH}
macx:target.path = /Applications

win32:target.path = c:/musrfit/bin
INSTALLS += target

# This is only needed for SL5.1
exists ( /opt/fontconfig-2.8.0/lib/libfontconfig.so ) {
  unix:LIBS += -L/opt/fontconfig-2.8.0/lib/ -lfontconfig
}

exists( /usr/bin/cygwin1.dll ) {
      QMAKE_CXXFLAGS += -D_WIN32GCC
      QMAKE_LFLAGS_SHAPP = -Wl,--enable-auto-import
}

QMAKE_CC = gcc
QMAKE_CXX = g++

# install path for the XML configuration file
unix:xml.path = $${MUSREDIT_INSTALL_PATH}
macx:xml.path = /Applications/musredit.app/Contents/Resources
win32:xml.path = c:/musrfit/bin
xml.files = musredit_startup.xml
INSTALLS += xml

CONFIG += qt \
    warn_on \
    release

QT += xml
QT += webkit
QT += network

HEADERS = musredit.h \
    PHelp.h \
    PTextEdit.h \
    PSubTextEdit.h \
    PAdmin.h \
    PFindDialog.h \
    PReplaceDialog.h \
    PReplaceConfirmationDialog.h \
    PFitOutputHandler.h \
    PPrefsDialog.h \
    PGetDefaultDialog.h \
    PGetTitleBlockDialog.h \ 
    PGetParameterBlockDialog.h \
    PGetTheoryBlockDialog.h \
    PGetFunctionsBlockDialog.h \
    PGetAsymmetryRunBlockDialog.h \
    PGetSingleHistoRunBlockDialog.h \
    PGetNonMusrRunBlockDialog.h \
    PGetFourierBlockDialog.h \
    PGetPlotBlockDialog.h \
    PMsr2DataDialog.h \
    PMusrEditAbout.h

SOURCES = PHelp.cpp \ 
    PTextEdit.cpp \
    PSubTextEdit.cpp \
    PAdmin.cpp \
    PFindDialog.cpp \
    PReplaceDialog.cpp \
    PReplaceConfirmationDialog.cpp \
    PFitOutputHandler.cpp \
    PPrefsDialog.cpp \
    PGetDefaultDialog.cpp \
    PGetTitleBlockDialog.cpp \ 
    PGetParameterBlockDialog.cpp \
    PGetTheoryBlockDialog.cpp \
    PGetFunctionsBlockDialog.cpp \
    PGetAsymmetryRunBlockDialog.cpp \
    PGetSingleHistoRunBlockDialog.cpp \
    PGetNonMusrRunBlockDialog.cpp \
    PGetFourierBlockDialog.cpp \
    PGetPlotBlockDialog.cpp \
    PMsr2DataDialog.cpp \
    PMusrEditAbout.cpp \
    main.cpp 

FORMS = forms/PFindDialog.ui \
    forms/PReplaceDialog.ui \
    forms/PReplaceConfirmationDialog.ui \
    forms/PGetDefaultDialog.ui \
    forms/PMusrEditAbout.ui \
    forms/PPrefsDialog.ui \
    forms/PGetTitleBlockDialog.ui \
    forms/PGetParameterBlockDialog.ui \
    forms/PGetTheoryBlockDialog.ui \
    forms/PGetFunctionsBlockDialog.ui \
    forms/PGetAsymmetryRunBlockDialog.ui \
    forms/PGetSingleHistoRunBlockDialog.ui \
    forms/PGetNonMusrRunBlockDialog.ui \
    forms/PGetFourierBlockDialog.ui \
    forms/PGetPlotBlockDialog.ui \
    forms/PMsr2DataDialog.ui

RESOURCES = musredit.qrc
