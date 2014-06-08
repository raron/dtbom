#-------------------------------------------------
#
# Project created by QtCreator 2014-05-04T14:16:25
#
#-------------------------------------------------

QT       += core gui webkit webkitwidgets script sql


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dtbom
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dtascloader.cpp \
    dtascnode.cpp \
    webshopscript.cpp \
    loadform.cpp \
    importcsv.cpp

HEADERS  += mainwindow.h \
    dtascloader.h \
    dtascnode.h \
    webshopscript.h \
    loadform.h \
    importcsv.h

FORMS    += mainwindow.ui \
    loadform.ui \
    importcsv.ui

QMAKE_CXXFLAGS += -std=c++11

VERSION = 0.2.0
DB_VERSION = 1

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += DB_VERSION=$$DB_VERSION

OTHER_FILES += \
    scripts/lomex.dtboms \
    scripts/digikey.dtboms \
    scripts/mouser.dtboms \
    scripts/soselectronic.dtboms \
    scripts/farnell.dtboms
