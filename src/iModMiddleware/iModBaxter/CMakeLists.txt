#get all sourcefiles for iModBaxter-Library
file(GLOB iModBaxter_SRC "*.cpp")

#build library iModBaxter
add_library(iModBaxter SHARED ${iModBaxter_SRC})
#SET_TARGET_PROPERTIES(iModBaxter PROPERTIES IMPORTED_LOCATION )
INCLUDE_DIRECTORIES(RobotRaconteur-0.5-OSX/include ${Boost_INCLUDE_DIR})

#Add unit test
add_executable(iModBaxterUnitTest ./iModBaxterUnitTest.cpp)

message(STATUS "Working Directory: ${CMAKE_SOURCE_DIR}")

#Building libiModBaxter
target_link_libraries(iModBaxter utils crypto ${CMAKE_SOURCE_DIR}/src/iModMiddleware/iModBaxter/RobotRaconteur-0.5-OSX/lib/libRobotRaconteur2.a ${Boost_LIBRARIES} )

#linking library
target_link_libraries(iModBaxterUnitTest iModBaxter)
