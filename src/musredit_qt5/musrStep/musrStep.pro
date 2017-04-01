TEMPLATE = app
TARGET = musrStep

# install path for musrStep via given prefix
count( PREFIX, 1 ) {
	MUSRSTEP_INSTALL_PATH = $${PREFIX}/bin
}
isEmpty( MUSRSTEP_INSTALL_PATH ) {
	MUSR_FIT_PATH = $$(MUSRFITPATH)
	count( MUSR_FIT_PATH, 1) {
		MUSRSTEP_INSTALL_PATH = $$(MUSRFITPATH)
	}
}
isEmpty( MUSRSTEP_INSTALL_PATH ) {
	ROOT_SYS_PATH = $$(ROOTSYS)
	count( ROOT_SYS_PATH, 1) {
		MUSRSTEP_INSTALL_PATH = $$(ROOTSYS)/bin
	}
}
isEmpty( MUSRSTEP_INSTALL_PATH ) {
	MUSRSTEP_INSTALL_PATH = /usr/local/bin
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
}

# install path for the application
unix:target.path = $$(ROOTSYS)/bin
macx:target.path = /Applications
win32:target.path = c:/musrfit/bin

INSTALLS += target

CONFIG += qt \
    warn_on \
    release \
    console

QT += widgets
QT += svg

INCLUDEPATH += "../../include"

HEADERS = ../../include/git-revision.h \
          PMusrStep.h

SOURCES = PMusrStep.cpp \
          musrStep.cpp

RESOURCES = musrStep.qrc

macx:ICON = icons/musrStep.icns
