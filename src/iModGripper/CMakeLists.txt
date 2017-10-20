#cmake_minimum_required(VERSION 2.8)

#project(iModGripper)

#SET(CMAKE_MODULE_PATH
#	    "${CMAKE_MODULE_PATH}"
#		"${iModGripper_SOURCE_DIR}/../CMakeModules")

#find MySQL
#find_package(MySQL REQUIRED)

#compiled executables to bin
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#compiled librarys to lib
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#get all sourcefiles for iModGripper-Library
file(GLOB iModGripperSRC "gripper/*.cpp" "c_interface.cpp")
set(modbus_dir /usr/local/include)

# find modbus
include(cmake_modules/FindMODBUS.cmake)


#build library iModGripper
add_library(iModGripper SHARED  ${iModGripperSRC})

#add some directorys -> are they really needed?!
include_directories(
	../utils/
	${modbus_dir}
	${Boost_INCLUDE_DIR}
)

#Building libiModGripper
target_link_libraries(iModGripper utils ${MODBUS_LIBRARIES})

# UnitTest
add_executable(iModGripperUnitTest ./unitTest.cpp)
target_link_libraries(iModGripperUnitTest iModGripper)

# Command line Interface 
add_executable(iModGripperCMDInterface ./CMD_Interface.cpp)
target_link_libraries(iModGripperCMDInterface iModGripper  ${Boost_LIBRARIES} utils)