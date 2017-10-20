SET(CMAKE_AUTOMOC ON)

SET(art_SOURCES ./imodview.cpp ./main.cpp)
SET(art_FORMS ./imodview.ui)

include_directories(../iModControll)
include_directories(../../iModGripper)
cmake_policy(SET CMP0015 NEW)
LINK_DIRECTORIES(../../../lib)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Gui REQUIRED)

qt5_wrap_ui(art_UIC_FORMS ${art_FORMS})

add_executable(iModView ${art_SOURCES} ${art_UIC_FORMS})

if(IMOD_GRIPPER)
	target_link_libraries(iModView Qt5::Widgets Qt5::Core Qt5::Network Qt5::Gui iModControl iModGripper)
else()
	target_link_libraries(iModView Qt5::Widgets Qt5::Core Qt5::Network Qt5::Gui iModControl)
endif()
