QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle

TARGET = libiModWii

TEMPLATE = lib

SOURCES += \
    EventContainer.cpp \
    Wii.cpp \
    Wii_CInterface.cpp \
    ../../external/wiiuse-master/src/classic.c \
    ../../external/wiiuse-master/src/dynamics.c \
    ../../external/wiiuse-master/src/events.c \
    ../../external/wiiuse-master/src/guitar_hero_3.c \
    ../../external/wiiuse-master/src/io.c \
    ../../external/wiiuse-master/src/ir.c \
    ../../external/wiiuse-master/src/motion_plus.c \
    ../../external/wiiuse-master/src/nunchuk.c \
    ../../external/wiiuse-master/src/os_nix.c \
    ../../external/wiiuse-master/src/os_win.c \
    ../../external/wiiuse-master/src/util.c \
    ../../external/wiiuse-master/src/wiiboard.c \
    ../../external/wiiuse-master/src/wiiuse.c

HEADERS += \ 
    EventContainer.h \
    Wii.h \
    ../../external/wiiuse-master/src/classic.h \
    ../../external/wiiuse-master/src/definitions.h \
    ../../external/wiiuse-master/src/definitions_os.h \
    ../../external/wiiuse-master/src/dynamics.h \
    ../../external/wiiuse-master/src/events.h \
    ../../external/wiiuse-master/src/guitar_hero_3.h \
    ../../external/wiiuse-master/src/io.h \
    ../../external/wiiuse-master/src/ir.h \
    ../../external/wiiuse-master/src/motion_plus.h \
    ../../external/wiiuse-master/src/nunchuk.h \
    ../../external/wiiuse-master/src/os.h \
    ../../external/wiiuse-master/src/wiiboard.h \
    ../../external/wiiuse-master/src/wiiuse.h \
    ../../external/wiiuse-master/src/wiiuse_internal.h \
    ../../external/wiiuse-master/src/wiiuse_msvcstdint.h


unix:!macx: INCLUDEPATH += "/home/hunabku/Programs/InteractionModels/CPP/external/wiiuse-master/src/"
win32: INCLUDEPATH += "P:\Boost\boost_1_60_0"
win32: INCLUDEPATH += "..\..\external\wiiuse-master\src"
win32: LIBS += -LP:\Boost\boost_1_60_0\lib64-msvc-12.0 -lWs2_32 -lsetupapi -lhid -LC:\Users\Hunabku\Desktop\ImodBuild\Utils\release -liModUtils

unix:!macx: LIBS += -L$$PWD/../../lib/ -lutils -lboost_system -lboost_thread -lboost_chrono

INCLUDEPATH += $$PWD/../utils
DEPENDPATH += $$PWD/../utils
