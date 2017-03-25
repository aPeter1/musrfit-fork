TEMPLATE = app
TARGET = musrStep

# install path for the application
unix:target.path = $$(ROOTSYS)/bin
INSTALLS += target

CONFIG += qt \
    warn_on \
    debug \
CONFIG += console

QT += widgets
QT += svg

INCLUDEPATH += "../../include"

HEADERS = ../../include/git-revision.h \
          PMusrStep.h

SOURCES = PMusrStep.cpp \
          musrStep.cpp

RESOURCES = musrStep.qrc

