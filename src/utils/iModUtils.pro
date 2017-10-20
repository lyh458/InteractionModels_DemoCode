#QT += core gui network
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT -= core gui

CONFIG += c++11

TARGET = iModUtils

TEMPLATE = lib

SOURCES += \ 
    Configuration.cpp

unix:  SOURCES +=  notify.c
win32: SOURCES += notify_windows.cpp

HEADERS += \
    Configuration.h \
    Timer.h

unix:  HEADERS +=  notify.h
win32: HEADERS += notify_windows.h

win32: INCLUDEPATH += "P:\Boost\boost_1_60_0"
win32: LIBS += -LP:\Boost\boost_1_60_0\lib64-msvc-12.0 -lWs2_32
