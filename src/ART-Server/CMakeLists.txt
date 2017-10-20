#get all sourcefiles for ARTServer

SET(CMAKE_AUTOMOC ON)

SET(art_SOURCES ./artnetworkclient.cpp ./main.cpp ./artringbuffer.cpp ./artnetwork.cpp ./artdatabase.cpp ./art.cpp ./artconfiguration.cpp ./artconfigurationpage.cpp ./artconverter.cpp)
SET(art_FORMS art.ui)

find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Sql)
find_package(Qt5Network)
find_package(Qt5Gui)

qt5_wrap_ui(art_UIC_FORMS ${art_FORMS})

add_executable(ARTServer WIN32 ${art_SOURCES} ${art_UIC_FORMS})
target_link_libraries(ARTServer Qt5::Widgets Qt5::Core Qt5::Sql Qt5::Network Qt5::Gui ${MYSQL_CLIENT_LIBS})

#add_executable(ARTCServer WIN32 ${art_SOURCES})
#target_link_libraries(ARTCServer Qt5::Widgets Qt5::Core Qt5::Sql Qt5::Network Qt5::Gui ${MYSQL_CLIENT_LIBS})
#set_target_properties(ARTCServer PROPERTIES COMPILE_DEFINITIONS "CONSOLE_MODE")
#add_dependencies(ARTCServer ARTServer)
