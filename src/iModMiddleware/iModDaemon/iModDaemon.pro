#-------------------------------------------------
#
# Project created by QtCreator 2015-05-14T16:00:12
#
#-------------------------------------------------

QT       += core network sql gui widgets

TARGET = iModDaemon
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/include/eigen3

SOURCES += main.cpp \
    imodutile.cpp \
    imodart.cpp \
    imoddatabase.cpp \
    imoddaemon.cpp \
    ../iModProtocol/iMod.protocol.cpp \
    imodmathutils.cpp \
    imodringbuffer.cpp \
    Kinect/imodkinect.cpp \
    imodartclient.cpp \
    Kinect/kvmainwindow.cpp \
    Kinect/kvkinect.cpp \
    imodcontrol_daemon.cpp \
    Robot/imodrobot.cpp \
    Robot/imodur5.cpp \
    Robot/imodftsensor.cpp \
    Robot/imodgripper.cpp \
    RobotiqFT150/rq_sensor_com.c \
    RobotiqFT150/rq_sensor_state.c

HEADERS += \
    imodutile.h \
    imodart.h \
    imoddatabase.h \
    imoddaemon.h \
    ../iModProtocol/iMod.protocol.h \
    imodmathutils.h \
    imodringbuffer.h \
    Kinect/imodkinect.h \
    imodartclient.h \
    Kinect/kvmainwindow.h \
    Kinect/kvkinect.h \
    Kinect/imodkinecttypes.h \
    imodcontrol_daemon.h \
    RobotiqFT150/rq_int.h \
    RobotiqFT150/rq_sensor_com.h \
    RobotiqFT150/rq_sensor_state.h \
    Robot/imodrobot.h \
    Robot/imodur5.h \
    Robot/imodftsensor.h \
    Robot/imodgripper.h

DISTFILES += \
    ../build/deamon.ini \
    ../../configuration.ini

win32:LIBS += -L"C:\Program Files\MySQL\MySQL Connector C 6.1\lib" -llibmysql
win32:INCLUDEPATH += "C:\Program Files\MySQL\MySQL Connector.C 6.1\include"
win32:DEPENDPATH += "C:\Program Files\MySQL\MySQL Connector.C 6.1\include"

win32:INCLUDEPATH += P:\Eigen3

#win32:LIBS += -L"C:\Program Files\MySQL\MySQL Connector.C 6.1\lib" -llibmysql
#win32:INCLUDEPATH += "C:\Program Files\MySQL\MySQL Connector.C 6.1\include"
#win32:DEPENDPATH += "C:\Program Files\MySQL\MySQL Connector.C 6.1\include"
