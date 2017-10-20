#cmake_minimum_required(VERSION 2.8)

#project(iModTracker)

#SET(CMAKE_MODULE_PATH 
#	    "${CMAKE_MODULE_PATH}"
#		"${iModTracker_SOURCE_DIR}/../CMakeModules")

#find MySQL
#find_package(MySQL REQUIRED)

#compiled executables to bin
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#compiled librarys to lib
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#get all sourcefiles for iModTracker-Library
file(GLOB iModTracker_SRC "*.cpp" "Lib/*.cpp")

#build library iModTracker
add_library(iModTracker SHARED  ${iModTracker_SRC})

    #add_executable(iModTrackerUnitTest ./iModTrackerUnitTest.cpp)

    #add some directorys -> are they realy needed?!
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

	#Building libiModTracker
	target_link_libraries(iModTracker utils ${MYSQL_CLIENT_LIBS} ${Boost_LIBRARIES})

    #linking library
    #target_link_libraries(iModTrackerUnitTest iModTracker)
