#-------------------------------------------------
#
# Project created by QtCreator 2015-10-12T14:10:14
#
#   (c) 2015 Alessandro Mauro <www.maetech.it>
#   icons: (C) 2013 Yusuke Kamiyamane.
#   All rights reserved.
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serialsurveyor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logmodel.cpp \
    countermodel.cpp \
    serialdevice.cpp \
    dlgsetup.cpp \
    dlgexport.cpp \
    temperaturemonitormodel.cpp

HEADERS  += mainwindow.h \
    logmodel.h \
    countermodel.h \
    serialdevice.h \
    dlgsetup.h \
    dlgexport.h \
    temperaturemonitormodel.h

FORMS    += mainwindow.ui \
    dlgsetup.ui \
    dlgexport.ui

RESOURCES += \
    common.qrc
