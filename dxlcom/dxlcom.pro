#-------------------------------------------------
#
# Project created by QtCreator 2016-03-12T20:24:44
#
#-------------------------------------------------

include(../defaults.pri)

QT       -= gui
QT       += serialport
CONFIG   += qt

TARGET = dxlcom
TEMPLATE = lib

DEFINES += DXLCOM_LIBRARY

SOURCES += \
        dxlcom.cpp \
        dxl.cpp \
        dxlax.cpp \
        dxlmx.cpp \

HEADERS +=\
        dxlcom_global.h \
        dxlcom.h \
        dxl.h \
        dxlConfig.h \
        dxlax.h \
        dxlmx.h\

unix {
    target.path = /usr/lib
    INSTALLS += target
}
