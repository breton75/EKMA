#-------------------------------------------------
#
# Project created by QtCreator 2017-06-23T15:46:13
#
#-------------------------------------------------

QT       += core gui network printsupport
#CONFIG += console
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GA
TEMPLATE = app

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
    ../../Common/sv_settings.cpp \
    sv_graph.cpp \
    ../../Common/qcustomplot/qcustomplot.cpp \
    sv_gawidget.cpp \
    ../general/sv_gathread.cpp

HEADERS  += mainwindow.h \
    ../../Common/sv_settings.h \
    sv_graph.h \
    ../../Common/qcustomplot/qcustomplot.h \
    sv_gawidget.h \
    ../general/sv_gathread.h

FORMS    += mainwindow.ui \
    sv_chartwidget.ui \
    sv_graphparamsdialog.ui

INCLUDEPATH += ../general
INCLUDEPATH += ../../Common/qcustomplot

RESOURCES += \
    ../general/qq.qrc