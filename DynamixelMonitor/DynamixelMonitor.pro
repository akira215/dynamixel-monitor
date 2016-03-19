#-------------------------------------------------
#
# Project created by QtCreator 2016-02-27T11:52:00
#
#-------------------------------------------------

include(../defaults.pri)

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = DynamixelMonitor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        portcombobox.cpp \
        dxlbaudratecombobox.cpp

HEADERS  += mainwindow.h \
        portcombobox.h \
        dxlbaudratecombobox.h

FORMS    += mainwindow.ui


#win32:CONFIG(release, debug|release): LIBS += -ldxl
#else:win32:CONFIG(debug, debug|release): LIBS += -ldxl
#else:unix: LIBS += -ldxl

LIBS += -L../dxlcom -ldxlcom

INCLUDEPATH += /usr/include
DEPENDPATH += /usr/include
