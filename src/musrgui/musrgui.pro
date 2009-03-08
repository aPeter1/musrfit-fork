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

HEADERS = PTextEdit.h \
    PSubTextEdit.h \
    PAdmin.h \
    PFitOutputHandler.h \
    PPrefsDialog.h \
    PGetDefaultDialog.h \
    PGetParameterDialog.h \
    PGetAsymmetryRunBlockDialog.h \
    PGetFourierDialog.h \
    PMlog2DbDialog.h \
    PGetPlotDialog.h

SOURCES = PTextEdit.cpp \
    PSubTextEdit.cpp \
    PAdmin.cpp \
    PFitOutputHandler.cpp \
    PPrefsDialog.cpp \
    PGetDefaultDialog.cpp \
    PGetParameterDialog.cpp \
    PGetAsymmetryRunBlockDialog.cpp \
    PGetFourierDialog.cpp \
    PGetPlotDialog.cpp \
    PMlog2DbDialog.cpp \
    main.cpp 

FORMS = forms/PGetDefaultDialogBase.ui \
    forms/PMusrGuiAbout.ui \
    forms/PPrefsDialogBase.ui \
    forms/PGetTitleDialog.ui \
    forms/PGetParameterDialogBase.ui \
    forms/PGetAsymmetryRunBlockDialogBase.ui \
    forms/PGetFourierDialogBase.ui \
    forms/PGetPlotDialogBase.ui \
    forms/PMlog2DbDialogBase.ui

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
    images/musrmlog2db.xpm \
    images/musrview.xpm \
    images/musrt0.xpm \
    images/musrprefs.xpm
