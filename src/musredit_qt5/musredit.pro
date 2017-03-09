TEMPLATE = app
TARGET = musredit

QMAKE_CXXFLAGS += -std=c++11

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
      QMAKE_LFLAGS_APP = -Wl,--enable-auto-import
}

isEmpty( CC ) {
    CC = gcc
}

isEmpty( CXX ) {
    CXX = g++
}

QMAKE_CC = $${CC}
QMAKE_CXX = $${CXX}
QMAKE_LINK = $${CXX}

# set proper permission for Mac OSX
macx {
   QMAKE_INSTALL_FILE = install -m 6755 -p -o $$(USER) -g staff
   QMAKE_INSTALL_PROGRAM = install -m 6755 -p -o root -g admin
}

# install path for the XML configuration file
unix:xml.path = $$(HOME)/.musrfit/musredit
macx:xml.path = $$(HOME)/.musrfit/musredit
win32:xml.path = c:/musrfit/bin

CONFIG += qt \
    warn_on \
    release

QT += widgets
QT += xml
QT += webkitwidgets
QT += network
QT += printsupport
QT += svg

INCLUDEPATH += "../include"

HEADERS = musredit.h \
    musrfit-info.h \
    PHelp.h \
    PTextEdit.h \
    PSubTextEdit.h \
    PAdmin.h \
    PFindDialog.h \
    PReplaceDialog.h \
    PReplaceConfirmationDialog.h \
    PFitOutputHandler.h \
    PDumpOutputHandler.h \
    PPrefsDialog.h \
    PGetMusrFTOptionsDialog.h \
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
    PChangeDefaultPathsDialog.h \
    PMusrEditAbout.h \
    ../include/git-revision.h

SOURCES = PHelp.cpp \ 
    PTextEdit.cpp \
    PSubTextEdit.cpp \
    PAdmin.cpp \
    PFindDialog.cpp \
    PReplaceDialog.cpp \
    PReplaceConfirmationDialog.cpp \
    PFitOutputHandler.cpp \
    PDumpOutputHandler.cpp \
    PPrefsDialog.cpp \
    PGetMusrFTOptionsDialog.cpp \
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
    PChangeDefaultPathsDialog.cpp \
    PMusrEditAbout.cpp \
    main.cpp 

FORMS = forms/PFindDialog.ui \
    forms/PReplaceDialog.ui \
    forms/PReplaceConfirmationDialog.ui \
    forms/PMusrEditAbout.ui \
    forms/PPrefsDialog.ui \
    forms/PGetMusrFTOptionsDialog.ui \
    forms/PGetTitleBlockDialog.ui \
    forms/PGetParameterBlockDialog.ui \
    forms/PGetTheoryBlockDialog.ui \
    forms/PGetFunctionsBlockDialog.ui \
    forms/PGetAsymmetryRunBlockDialog.ui \
    forms/PGetSingleHistoRunBlockDialog.ui \
    forms/PGetNonMusrRunBlockDialog.ui \
    forms/PGetFourierBlockDialog.ui \
    forms/PGetPlotBlockDialog.ui \
    forms/PMsr2DataDialog.ui \
    forms/PChangeDefaultPathsDialog.ui

RESOURCES = musredit.qrc

macx:ICON = icons/musredit.icns

