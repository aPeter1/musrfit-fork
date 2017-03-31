TEMPLATE = app
TARGET = musrWiz

# install path for the application
unix:target.path = $$(ROOTSYS)/bin
macx:target.path = /Applications
win32:target.path = c:/musrfit/bin

INSTALLS += target

# install path for the XML instrument def file
unix:instrumendDef.path = $$(HOME)/.musrfit/musrWiz
macx:instrumendDef.path = $$(HOME)/.musrfit/musrWiz
instrumendDef.files = instrument_defs/instrument_def_psi.xml
exists( $$(HOME)/.musrfit/musrWiz/instrument_def_psi.xml ) {
  unix:instrumendDef.extra =  mv $$(HOME)/.musrfit/musrWiz/instrument_def_psi.xml $$(HOME)/.musrfit/musrWiz/instrument_def_psi.xml.backup 
  macx:instrumendDef.extra =  mv $$(HOME)/.musrfit/musrWiz/instrument_def_psi.xml $$(HOME)/.musrfit/musrWiz/instrument_def_psi.xml.backup 
}
INSTALLS += instrumendDef

# install path for the XML musrfit funcs file
unix:musrfitFunc.path = $$(HOME)/.musrfit/musrWiz
macx:musrfitFunc.path = $$(HOME)/.musrfit/musrWiz
musrfitFunc.files = func_defs/musrfit_funcs.xml
exists( $$(HOME)/.musrfit/musrWiz/musrfit_funcs.xml ) {
  unix:musrfitFunc.extra = mv $$(HOME)/.musrfit/musrWiz/musrfit_funcs.xml $$(HOME)/.musrfit/musrWiz/musrfit_funcs.xml.backup 
  macx:musrfitFunc.extra = mv $$(HOME)/.musrfit/musrWiz/musrfit_funcs.xml $$(HOME)/.musrfit/musrWiz/musrfit_funcs.xml.backup 
}
INSTALLS += musrfitFunc

# install path for the musrWiz defaults XML
unix:musrWizDefault.path = $$(HOME)/.musrfit/musrWiz
macx:musrWizDefault.path = $$(HOME)/.musrfit/musrWiz
musrWizDefault.files = musrWiz.xml
exists( $$(HOME)/.musrfit/musrWiz/musrWiz.xml ) {
  unix:musrWizDefault.extra = mv $$(HOME)/.musrfit/musrWiz/musrWiz.xml $$(HOME)/.musrfit/musrWiz/musrWiz.xml.backup
  macx:musrWizDefault.extra = mv $$(HOME)/.musrfit/musrWiz/musrWiz.xml $$(HOME)/.musrfit/musrWiz/musrWiz.xml.backup
}
INSTALLS += musrWizDefault

CONFIG += qt \
    warn_on \
    release

QT += widgets
QT += xml
QT += core
QT += svg

# set proper permission for Mac OSX
macx {
   QMAKE_INSTALL_FILE = install -m 6755 -p -o $$(USER) -g staff
   QMAKE_INSTALL_PROGRAM = install -m 6755 -p -o root -g admin
}

INCLUDEPATH += "../../include"

HEADERS = musrWiz.h \
          PTheoTemplate.h \
          PMusrfitFunc.h \
          PInstrumentDef.h \
          PAdmin.h \
          PMusrWiz.h

SOURCES = PTheoTemplate.cpp \
          PMusrfitFunc.cpp \
          PInstrumentDef.cpp \
          PAdmin.cpp \
          PMusrWiz.cpp \
          musrWiz.cpp

RESOURCES = musrWiz.qrc

macx:ICON = icons/musrWiz.icns