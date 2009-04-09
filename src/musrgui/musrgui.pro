TEMPLATE = app
TARGET = musrgui
target.path = $$(HOME)/analysis/bin
INSTALLS += target

# install path for the XML configuration file
unix:xml.path = $$(HOME)/analysis/bin/
win32:xml.path = $$(HOME)/analysis/bin/
xml.files = musrgui_startup.xml
INSTALLS += xml

CONFIG += qt \
    warn_on \
    debug

HEADERS = musrgui.h \
    PTextEdit.h \
    PSubTextEdit.h \
    PAdmin.h \
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

SOURCES = PTextEdit.cpp \
    PSubTextEdit.cpp \
    PAdmin.cpp \
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

FORMS = forms/PGetDefaultDialogBase.ui \
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
    images/filenew.xpm \
    images/fileopen.xpm \
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
