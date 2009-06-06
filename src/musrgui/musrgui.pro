TEMPLATE = app
TARGET = musrgui
target.path = $$(HOME)/analysis/bin
INSTALLS += target
QMAKE_CC = gcc
QMAKE_CXX = g++
QMAKE_LFLAGS_SHAPP = -Wl,--enable-auto-import

# install path for the XML configuration file
unix:xml.path = $$(HOME)/analysis/bin/
win32:xml.path = $$(HOME)/analysis/bin/
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
