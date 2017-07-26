#-------------------------------------------------
#
# Project created by QtCreator 2017-07-20T09:37:06
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gaemulator
TEMPLATE = app
CONFIG += c++11

VERSION = 1.0.0    # major.minor.patch

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    ../general/sv_gathread.cpp \
    ../../svlib/sv_log.cpp \
    ../../svlib/sv_settings.cpp

HEADERS  += mainwindow.h \
    ../general/sv_gathread.h \
    ../../svlib/sv_log.h \
    ../../svlib/sv_settings.h

FORMS    += mainwindow.ui
