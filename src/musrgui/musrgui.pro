TEMPLATE = app
TARGET = musrgui

!exists( musrgui_startup.xml ) {
    error( "Configuration file musrgui_startup.xml not found! Please configure musrfit first, before trying to install musrgui!" )
}

# install path for musrgui
count( PREFIX, 1 ) {
    MUSRGUI_INSTALL_PATH = $${PREFIX}/bin
}
isEmpty( MUSRGUI_INSTALL_PATH ) {
    MUSR_FIT_PATH = $$(MUSRFITPATH)
    count( MUSR_FIT_PATH, 1 ) {
        MUSRGUI_INSTALL_PATH = $$(MUSRFITPATH)
    }
}
isEmpty( MUSRGUI_INSTALL_PATH ) {
    ROOT_SYS_PATH = $$(ROOTSYS)
    count( ROOT_SYS_PATH, 1 ) {
        MUSRGUI_INSTALL_PATH = $$(ROOTSYS)/bin
    }
}
isEmpty( MUSRGUI_INSTALL_PATH ) {
    MUSRGUI_INSTALL_PATH = /usr/local/bin
}

unix {
    message( "Determined installation path: $${MUSRGUI_INSTALL_PATH}" )
}

unix:target.path = $${MUSRGUI_INSTALL_PATH}

macx:target.path = /Applications

exists( /usr/bin/cygwin1.dll ) {
    QMAKE_LFLAGS_SHAPP = -Wl,--enable-auto-import
}

INSTALLS += target
QMAKE_CC = gcc
QMAKE_CXX = g++

# install path for the XML configuration file
unix:xml.path = $${MUSRGUI_INSTALL_PATH}
macx:xml.path = $${MUSRGUI_INSTALL_PATH}
xml.files = musrgui_startup.xml
INSTALLS += xml

CONFIG += qt \
    warn_on \
    debug

HEADERS = musrgui.h \
    PFileWatcher.h \
    PTextEdit.h \
    PSubTextEdit.h \
    PAdmin.h \
    PFindDialog.h \
    PReplaceDialog.h \
    PFitOutputHandler.h \
    PPrefsDialog.h \
    PGetDefaultDialog.h \
    PGetParameterDialog.h \
    PGetTheoryBlockDialog.h \
    PGetFunctionsBlockDialog.h \
    PGetAsymmetryRunBlockDialog.h \
    PGetSingleHistoRunBlockDialog.h \
    PGetNonMusrRunBlockDialog.h \
    PGetFourierDialog.h \
    PMsr2DataDialog.h \
    PGetPlotDialog.h

SOURCES = PFileWatcher.cpp \
    PTextEdit.cpp \
    PSubTextEdit.cpp \
    PAdmin.cpp \
    PFindDialog.cpp \
    PReplaceDialog.cpp \
    PFitOutputHandler.cpp \
    PPrefsDialog.cpp \
    PGetDefaultDialog.cpp \
    PGetParameterDialog.cpp \
    PGetTheoryBlockDialog.cpp \
    PGetFunctionsBlockDialog.cpp \
    PGetAsymmetryRunBlockDialog.cpp \
    PGetSingleHistoRunBlockDialog.cpp \
    PGetNonMusrRunBlockDialog.cpp \
    PGetFourierDialog.cpp \
    PGetPlotDialog.cpp \
    PMsr2DataDialog.cpp \
    main.cpp 

FORMS = forms/PFindDialogBase.ui \
    forms/PReplaceDialogBase.ui \
    forms/PReplaceConfirmationDialog.ui \
    forms/PGetDefaultDialogBase.ui \
    forms/PMusrGuiAbout.ui \
    forms/PPrefsDialogBase.ui \
    forms/PGetTitleDialog.ui \
    forms/PGetParameterDialogBase.ui \
    forms/PGetTheoryBlockDialogBase.ui \
    forms/PGetFunctionsBlockDialogBase.ui \
    forms/PGetAsymmetryRunBlockDialogBase.ui \
    forms/PGetSingleHistoRunBlockDialogBase.ui \
    forms/PGetNonMusrRunBlockDialogBase.ui \
    forms/PGetFourierDialogBase.ui \
    forms/PGetPlotDialogBase.ui \
    forms/PMsr2DataDialogBase.ui

IMAGES = images/editcopy.xpm \
    images/editcut.xpm \
    images/editpaste.xpm \
    images/editredo.xpm \
    images/editundo.xpm \
    images/editfind.xpm \
    images/editnext.xpm \
    images/editprevious.xpm \
    images/filenew.xpm \
    images/fileopen.xpm \
    images/filereload.xpm \
    images/fileprint.xpm \
    images/filesave.xpm \
    images/musrasym.xpm \
    images/musrsinglehisto.xpm \
    images/musrcalcchisq.xpm \
    images/musrfit.xpm \
    images/musrmsr2data.xpm \
    images/musrview.xpm \
    images/musrt0.xpm \
    images/musrprefs.xpm \
    images/musrswap.xpm
