TEMPLATE	= app

TARGET		= musrgui
target.path     = $$(HOME)/analysis/bin
INSTALLS        += target

# install path for the XML configuration file
unix:  xml.path = $$(HOME)/analysis/bin/
win32: xml.path = $$(HOME)/analysis/bin/
xml.files = musrgui_startup.xml
INSTALLS += xml

CONFIG		+= qt warn_on debug



HEADERS		= PTextEdit.h \
                  PAdmin.h \
                  PFitOutputHandler.h \
                  PGetDefaultDialog.h
		  
SOURCES		= PTextEdit.cpp \
                  PAdmin.cpp \
                  PFitOutputHandler.cpp \
                  PGetDefaultDialog.cpp \
		  main.cpp
		  
FORMS           = forms/PGetDefaultDialogBase.ui

IMAGES 		= editcopy.xpm        \
                  editcut.xpm         \
		  editpaste.xpm       \
		  editredo.xpm        \
		  editundo.xpm        \
		  filenew.xpm         \
		  fileopen.xpm        \
		  fileprint.xpm       \
		  filesave.xpm        \
		  musrasym.xpm        \
		  musrsinglehisto.xpm \
		  musrcalcchisq.xpm   \
		  musrfit.xpm         \
		  musrview.xpm        \
		  musrt0.xpm          \
		  musrprefs.xpm
		  
