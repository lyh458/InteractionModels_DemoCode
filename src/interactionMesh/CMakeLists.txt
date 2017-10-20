# find header files for including (e.g. for ides like xcode)
file(GLOB SRC_HEADER *.h)

#find_package(Qt5Core REQUIRED)
#find_package(Qt5Network REQUIRED)

#get all sourcefiles for iModMesh
set(SRC 
	#BlenderController.cpp
	#Controller.cpp
	#InteractionMeshController.cpp
	InteractionMeshControllerNetwork.cpp
	InteractionMeshControllerCInterface.cpp
	#MatlabController.cpp
	#MatlabInstance.cpp
	#MessageConverter.cpp
	#Pose.cpp
	#PoseContainer.cpp
	#TimedPoseContainer.cpp
	#Timer.cpp
    ${SRC_HEADER}
	)

INCLUDE_DIRECTORIES(
	../utils/
	#${Boost_INCLUDE_DIR}
	#${MATLAB_INCLUDE_DIR}
	${EIGEN3_INCLUDE_DIR}
)

#SET(CMAKE_CXX_FLAGS "-Wl,-rpath,/opt/MATLAB/R2016a/bin/glnxa64")

#Add final binary
add_library(iModMesh SHARED ${SRC})
add_executable(iModMeshUnitTest ./iModMeshUnitTest.cpp)

#target_link_libraries(iModMesh utils ${Boost_LIBRARIES} ${MATLAB_LIBRARIES} Qt5::Core Qt5::Network)
target_link_libraries(iModMesh utils)
target_link_libraries(iModMeshUnitTest iModMesh)
