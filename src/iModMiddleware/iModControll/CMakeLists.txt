SET(art_SOURCES ./../iModProtocol/iMod.protocol.cpp ./imodcontrol.cpp ./imodnetwork.cpp ./imodkinectv2.cpp ./imodcontrol_cinterface.cpp)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Gui REQUIRED)

add_library(iModControl SHARED ${art_SOURCES})
target_link_libraries(iModControl Qt5::Widgets Qt5::Core Qt5::Sql Qt5::Network Qt5::Gui ${MYSQL_CLIENT_LIBS} crypto)
