#-------------------------------------------------
#
# Project created by QtCreator 2015-06-04T09:44:47
#
#-------------------------------------------------

QT       += network gui

TARGET = iModControl
TEMPLATE = lib

DEFINES += IMODCONTROLL_LIBRARY

SOURCES += \
    imodnetwork.cpp \
    imodkinectv2.cpp \
    ../iModProtocol/iMod.protocol.cpp \
    imodcontrol.cpp \
    imodobjectdetection.cpp \
    imodcontrol_cinterface.cpp

HEADERS +=\
    imodcontroll_global.h \
    imodnetwork.h \
    imodkinectv2.h \
    ../iModProtocol/iMod.protocol.h \
    imodcontrol.h \
    imodobjectdetection.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:INCLUDEPATH += P:\Eigen3

win32:LIBS += -LP:\OpenCV\opencv\build\x64\vc12\lib -lopencv_world310
win32:INCLUDEPATH += P:\OpenCV\opencv\build\include

win32:LIBS += -L"C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409\Lib\x64" -lKinect20 -lKinect20.Face -lKinect20.Fusion -lKinect20.VisualGestureBuilder
win32:INCLUDEPATH += "C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409\inc"
win32:DEPENDPATH += "C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409\inc"
