#-------------------------------------------------
#
# Project created by QtCreator 2025-06-09T08:02:26
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SpiFlashReader
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp\
        src/serialport.cpp

HEADERS  += src/mainwindow.h\
            src/serialport.h

FORMS    += forms/mainwindow.ui
