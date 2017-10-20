#cmake_minimum_required(VERSION 2.8)

#project(iModMesh)

#SET(CMAKE_MODULE_PATH 
#	"${CMAKE_MODULE_PATH}"
#	"${iModMesh_SOURCE_DIR}/../CMakeModules")

#search for boost
#set(Boost_DEBUG ON)
#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_STATIC_RUNTIME ON)
#find_package(Boost COMPONENTS system thread timer date_time program_options REQUIRED)
#message(STATUS ${Boost_LIBRARIES})
#message(STATUS ${Boost_INCLUDE_DIR})

#Find Matlab 
#ifind_package(Matlab REQUIRED)

#Find NaoqiSDK
#find_package(NaoqiSDK REQUIRED)

#Find Eigen3
#find_package(Eigen3 REQUIRED)

#compiled executables to bin
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#get all sourcefiles for iModMesh
set(SRC 
	../interactionMesh/Controller.cpp
	../interactionMesh/MatlabController.cpp
	../interactionMesh/MatlabInstance.cpp
	../interactionMesh/MessageConverter.cpp
	../interactionMesh/Pose.cpp
	../interactionMesh/PoseContainer.cpp
	../interactionMesh/TimedPoseContainer.cpp
	../interactionMesh/Timer.cpp
	NaoController.cpp
	NaoControllerCInterface.cpp
	)

INCLUDE_DIRECTORIES(
	../interactionMesh
	${MATLAB_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${EIGEN3_INCLUDE_DIR}
	${NAOQISDK_INCLUDE_DIRS}
)

#Some compiler flags
#add_definitions(-v -std=c++11)

#Add final binary
add_library(iModMesh SHARED ${SRC})
target_link_libraries(iModMesh  ${NAOQISDK_LIBRARIES} ${MATLAB_LIBRARIES} ${Boost_LIBRARIES})
