TEMPLATE = app
TARGET = musredit
unix:target.path = $(ROOTSYS)/bin
macx:target.path = /Applications
win32:target.path = c:/musrfit/bin

exists( /usr/bin/cygwin1.dll ) {
      QMAKE_LFLAGS_SHAPP = -Wl,--enable-auto-import
}

INSTALLS += target
QMAKE_CC = gcc
QMAKE_CXX = g++

# install path for the XML configuration file
unix:xml.path = $(ROOTSYS)/bin/
macx:xml.path = $(ROOTSYS)/bin/
win32:xml.path = c:/musrfit/bin
xml.files = musredit_startup.xml
INSTALLS += xml

CONFIG += qt \
    warn_on \
    debug

QT += xml

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
