QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ART-Server
TEMPLATE = app

#DEFINES += CONSOLE_MODE

INCLUDEPATH += P:/InteractionModelsDependencies/Kinect/v1/inc/
INCLUDEPATH += P:/InteractionModelsDependencies/Kinect/v2/inc/

win32: LIBS += -LP:/InteractionModelsDependencies/Kinect/v1/lib/x64/ -lKinect10
win32: LIBS += -LP:/InteractionModelsDependencies/Kinect/v2/lib/x64/ -lKinect20
win32: LIBS += -LP:/InteractionModelsDependencies/Install_MySql/lib/ -llibmysql

SOURCES += main.cpp\
        art.cpp \
    artkinectwidget.cpp \
    artnetwork.cpp \
    artnetworkclient.cpp \
    artdatabase.cpp \
    artringbuffer.cpp \
    artkinectv1.cpp \
    artkinectv2.cpp \
    artkinectcalibrationwidget.cpp \
    artkinectbase.cpp \
    artconfiguration.cpp \
    artconfigurationpage.cpp \
    artconverter.cpp

HEADERS  += art.h \
    artkinectwidget.h \
    artnetwork.h \
    artnetworkclient.h \
    artdatabase.h \
    artringbuffer.h \
    artkinectv1.h \
    artkinectv2.h \
    artkinectcalibrationwidget.h \
    artkinectbase.h \
    artconfiguration.h \
    artconfigurationpage.h \
    artconverter.h

FORMS    += art.ui \
    artkinectcalibrationwidget.ui



