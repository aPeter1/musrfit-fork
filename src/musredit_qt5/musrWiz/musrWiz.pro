TEMPLATE = app
TARGET = musrWiz

# install path for musrWiz
count( PREFIX, 1 ) {
    MUSRWIZ_INSTALL_PATH = $${PREFIX}/bin
}
isEmpty( MUSRWIZ_INSTALL_PATH ) {
    MUSR_FIT_PATH = $$(MUSRFITPATH)
    count( MUSR_FIT_PATH, 1 ) {
        MUSRWIZ_INSTALL_PATH = $$(MUSRFITPATH)
    }
}
isEmpty( MUSRWIZ_INSTALL_PATH ) {
    ROOT_SYS_PATH = $$(ROOTSYS)
    count( ROOT_SYS_PATH, 1 ) {
        MUSRWIZ_INSTALL_PATH = $$(ROOTSYS)/bin
    }
}
isEmpty( MUSRWIZ_INSTALL_PATH ) {
    MUSRWIZ_INSTALL_PATH = /usr/local/bin
}

unix {
    message( "Determined installation path: $${MUSRWIZ_INSTALL_PATH}" )
}

unix:target.path = $${MUSRWIZ_INSTALL_PATH}
macx:target.path = /Applications
win32:target.path = c:/musrfit/bin

INSTALLS += target

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
