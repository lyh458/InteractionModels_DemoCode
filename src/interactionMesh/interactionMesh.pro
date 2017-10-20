QT -= core
QT -= gui

CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle

TARGET = interactionMesh

TEMPLATE = lib

INCLUDEPATH += $$PWD/../utils
INCLUDEPATH += /usr/include/eigen3

HEADERS += \
    #BlenderController.h \
    #Controller.h \
    #InteractionMeshController.h \
    #MatlabController.h \
    #MatlabInstance.h \
    #MessageConverter.h \
    #Pose.h \
    #PoseContainer.h \
    #TimedPoseContainer.h \
    #Timer.h \
    InteractionMeshControllerNetwork.h

SOURCES += \
    #BlenderController.cpp \
    #Controller.cpp \
    #InteractionMeshController.cpp \
    InteractionMeshControllerCInterface.cpp \
    #MatlabController.cpp \
    #MatlabInstance.cpp \
    #MessageConverter.cpp \
    #Pose.cpp \
    #PoseContainer.cpp \
    #TimedPoseContainer.cpp \
    #Timer.cpp \
    InteractionMeshControllerNetwork.cpp
