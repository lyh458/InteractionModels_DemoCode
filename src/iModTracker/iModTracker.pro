QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle

TARGET = libiModTracker

TEMPLATE = lib

SOURCES += \
    ART.cpp \
    ARTCInterface.cpp \
    DTrackSDK.cpp \
    Target.cpp \
    Lib/DTrackNet.cpp \
    Lib/DTrackParse.cpp \
    Lib/DTrackParser.cpp

HEADERS += \
    ART.h \
    DTrackSDK.hpp \
    Target.h \
    Lib/DTrackDataTypes.h \
    Lib/DTrackNet.h \
    Lib/DTrackParse.hpp \
    Lib/DTrackParser.hpp


win32: INCLUDEPATH += "P:\Boost\boost_1_60_0"
win32: LIBS += -LP:\Boost\boost_1_60_0\lib64-msvc-12.0 -lWs2_32 -LC:\Users\Hunabku\Desktop\ImodBuild\Utils\release -liModUtils

unix:!macx: LIBS += -L$$PWD/../../lib/ -lutils -lboost_system -lboost_thread -lboost_chrono

INCLUDEPATH += $$PWD/../utils
DEPENDPATH += $$PWD/../utils
