QT += core network

CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle

TARGET = interactionMeshUnitTest

TEMPLATE = app


INCLUDEPATH += $$PWD/../utils
INCLUDEPATH += /usr/include/eigen3

LIBS += -L/tmp/Build/build-interactionMesh-Desktop_GCC-Debug/
LIBS += -linteractionMesh
LIBS += -L/tmp/Build/build-iModUtils-Desktop_GCC-Debug/
LIBS += -liModUtils
LIBS += -lboost_system -lboost_thread -lboost_chrono -lcrypto

SOURCES += \
    iModMeshUnitTest.cpp
