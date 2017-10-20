SET(CMAKE_AUTOMOC ON)

SET(art_SOURCES ./../iModProtocol/iMod.protocol.cpp 
./imodmathutils.cpp
./imodutile.cpp
./imodringbuffer.cpp
./imodartclient.cpp
./main.cpp
./imoddatabase.cpp
./imoddaemon.cpp
./Kinect/kvmainwindow.cpp
./Kinect/kvkinect.cpp
./Kinect/imodkinect.cpp
./imodart.cpp
./Robot/imodrobot.cpp
./Robot/imodftsensor.cpp
./Robot/imodur5.cpp
./Robot/imodgripper.cpp
./imodcontrol_daemon.cpp
./RobotiqFT150/rq_sensor_state.c
./RobotiqFT150/rq_sensor_com.c
)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(EIGEN3_INCLUDE_DIR)
include_directories(/usr/include/eigen3)

add_executable(iModDaemon ${art_SOURCES} ${art_UIC_FORMS})
target_link_libraries(iModDaemon Qt5::Core Qt5::Gui Qt5::Sql Qt5::Network Qt5::Widgets ${MYSQL_CLIENT_LIBS} pthread)
