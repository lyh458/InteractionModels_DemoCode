#-------------------------------------------------
#
# Project created by QtCreator 2015-06-04T10:02:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iModView
TEMPLATE = app


SOURCES += main.cpp\
        imodview.cpp

HEADERS  += imodview.h

FORMS    += imodview.ui

unix:LIBS += -L/tmp/Build/build-iModControll-Desktop_GCC-Debug/ -liModControl
unix:INCLUDEPATH += /home/hunabku/Programs/InteractionModels/CPP/src/iModMiddleware/iModControll
unix:DEPENDPATH += /home/hunabku/Programs/InteractionModels/CPP/src/iModMiddleware/iModControll

win32:LIBS += -LC:\Users\Hunabku\Desktop\ImodBuild\Controll\release -liModControl
win32:INCLUDEPATH += P:\InteractionModels\CPP\src\iModMiddleware\iModControll
win32:DEPENDPATH += P:\InteractionModels\CPP\src\iModMiddleware\iModControll

