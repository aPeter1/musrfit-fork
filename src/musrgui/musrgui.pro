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
		  
FORMS           = forms/PGetDefaultDialogBase.ui \
                  forms/PMusrGuiAbout.ui

IMAGES 		= images/editcopy.xpm        \
                  images/editcut.xpm         \
		  images/editpaste.xpm       \
		  images/editredo.xpm        \
		  images/editundo.xpm        \
		  images/filenew.xpm         \
		  images/fileopen.xpm        \
		  images/fileprint.xpm       \
		  images/filesave.xpm        \
		  images/musrasym.xpm        \
		  images/musrsinglehisto.xpm \
		  images/musrcalcchisq.xpm   \
		  images/musrfit.xpm         \
		  images/musrview.xpm        \
		  images/musrt0.xpm          \
		  images/musrprefs.xpm
		  
