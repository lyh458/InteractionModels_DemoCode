cmake_minimum_required(VERSION 3.3)

find_package(Boost COMPONENTS system thread date_time chrono program_options regex filesystem REQUIRED)

#get all sourcefiles for iModBaxter-Library
file(GLOB DataServerClient_SRC "*.cpp")

#build library iModBaxter
#SET_TARGET_PROPERTIES(iModBaxter PROPERTIES IMPORTED_LOCATION )
INCLUDE_DIRECTORIES(../RobotRaconteur-0.5-OSX/include ${Boost_INCLUDE_DIR})

#Add unit test
add_executable(DataServerClient ${DataServerClient_SRC} )

message(STATUS "Working Directory: ${CMAKE_SOURCE_DIR}")

#linking library
target_link_libraries(DataServerClient ${CMAKE_SOURCE_DIR}/../../../../lib/libutils.dylib crypto ${Boost_LIBRARIES}  ${CMAKE_SOURCE_DIR}/../RobotRaconteur-0.5-OSX/lib/libRobotRaconteur2.a )
