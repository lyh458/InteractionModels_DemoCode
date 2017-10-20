QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iModSatellite
#CONFIG += console
#CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    imodsatellite.cpp

win32:LIBS += -LC:\Users\Hunabku\Desktop\ImodBuild\Controll\release -liModControl
win32:INCLUDEPATH += P:\InteractionModels\CPP\src\iModMiddleware\iModControll
win32:DEPENDPATH += P:\InteractionModels\CPP\src\iModMiddleware\iModControll

HEADERS += \
    imodsatellite.h

