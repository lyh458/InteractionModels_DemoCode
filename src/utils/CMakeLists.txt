#cmake_minimum_required(VERSION 2.8)

#project(utils)

#SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}:${utils_SOURCE_DIR}/CMakeModules")
SET(CMAKE_MODULE_PATH 
	"${CMAKE_MODULE_PATH}"
	"${utils_SOURCE_DIR}/../CMakeModules")

#compiled librarys to bin
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#get all sourcefiles for utils
file(GLOB utils_SRCCPP "*.cpp")
file(GLOB utils_SRCC "*.c")

set(utils_SRC
	${utils_SRCCPP}
	${utils_SRCC}
)

#build library utils
add_library(utils SHARED ${utils_SRC})

#build utils
include_directories(${Boost_INCLUDE_DIR})

#Building libConfiguration
target_link_libraries(utils ${Boost_LIBRARIES})

#Build Unit Test
#add_executable(iModUtilsUnitTest ./LogUnitTest.cpp)
#target_link_libraries(iModUtilsUnitTest utils)

