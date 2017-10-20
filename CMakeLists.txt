cmake_minimum_required(VERSION 2.8)
project(iMod)

option(IMOD_OSG  "Build OSG App" OFF)
option(IMOD_NAO  "Build NAO Lib" OFF)
option(IMOD_TRACKER  "Build ART Tracker Lib" ON)
option(IMOD_MESH  "Build Interaction Mesh Mesh Lib" OFF)
option(IMOD_UTILS  "Build Utils Lib" ON)
#option(IMOD_ARTSERVER  "Build ART Server" OFF)
option(IMOD_RIFT "Build Oculus Rift Plugin" OFF)
option(IMOD_CAVE "Build CAVE Plugin" OFF)
option(IMOD_HAPTIC  "Build Haptic Lib" OFF)
option(IMOD_CONTROLL  "Build iModMiddleware: Controller" OFF)
option(IMOD_DAEMON  "Build iModMiddleware: Daemon" OFF)
option(IMOD_VIEW "Build iModMiddleware: View" OFF)
option(IMOD_GRIPPER "Build iMod Gripper Controller" OFF)
option(IMOD_WII "Build iMod WII" OFF)
option(IMOD_BAXTER "Build iMod Baxter" OFF)

option(USE_OSGWORKS "Use Additional OSGWorks dependencies" OFF)

SET(CMAKE_AUTOMOC ON)

SET(CMAKE_MODULE_PATH
	"${CMAKE_MODULE_PATH}"
	"${iMod_SOURCE_DIR}/CMakeModules")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#Some compiler flags
set(CMAKE_CXX_FLAGS "-std=c++11")

#Global packages
#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREADED ON)
#unset(Boost_INCLUDE_DIR CACHE)
#unset(Boost_LIBRARY_DIRS CACHE)
#find_package(Boost 1.56.0 EXACT COMPONENTS system thread date_time chrono program_options regex filesystem REQUIRED)
find_package(Boost COMPONENTS system thread date_time chrono program_options regex filesystem REQUIRED)


if(IMOD_RIFT)
        find_package(OpenSceneGraph REQUIRED COMPONENTS osg osgViewer osgDB osgGA osgUtil)
        add_subdirectory(src/OculusRift)
endif()

if(IMOD_OSG)
	find_package(OpenSceneGraph REQUIRED COMPONENTS osg osgViewer osgDB osgGA osgUtil osgParticle osgAnimation osgShadow)
	find_package(OpenThreads)
	#find_package(Eigen3 REQUIRED)

	if (USE_OSGWORKS)
		find_package(OSGWorks)
	endif()

	message (STATUS "OSG Version is ${OPENSCENEGRAPH_VERSION} ")
	if ( OPENSCENEGRAPH_VERSION VERSION_LESS "3.2" )
		    add_definitions(-DOSG_VERSION_LESS_THAN_3_DOT_2 )
	endif ()
	add_subdirectory(src/iModOSG)

	# add a target to generate API documentation with Doxygen
	find_package(Doxygen)
		if(DOXYGEN_FOUND)
			configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
			add_custom_target(doc
			${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../
			COMMENT "Generating API documentation with Doxygen" VERBATIM
			)
		endif(DOXYGEN_FOUND)

endif()

if(IMOD_NAO)
	find_package(NaoqiSDK REQUIRED)
	find_package(Eigen3 REQUIRED)
	find_package(Matlab REQUIRED)
	add_subdirectory(src/iModNao)
endif()

if(IMOD_TRACKER)
	find_package(MySQL REQUIRED)
	add_subdirectory(src/iModTracker)
endif()

if(IMOD_HAPTIC)
	#find_package(WiiUse REQUIRED)
	find_package(Bullet REQUIRED)
    find_package(SDL REQUIRED)
	add_subdirectory(src/iModHaptic)
endif()

if(IMOD_MESH)
	find_package(Eigen3 REQUIRED)
	find_package(Matlab REQUIRED)
	add_subdirectory(src/interactionMesh)
	#add_subdirectory(src/libpredictIM)
endif()

#if(IMOD_ARTSERVER)
#    # finding packages is managed in the Sub-CMakeLists
#	find_package(MySQL REQUIRED)
#	add_subdirectory(src/ART-Server)
#endif()

if(IMOD_CONTROLL)
    # finding packages is managed in the Sub-CMakeLists
	add_subdirectory(src/iModMiddleware/iModControll)
endif()

if(IMOD_DAEMON)
    # finding packages is managed in the Sub-CMakeLists
	find_package(MySQL REQUIRED)
	add_subdirectory(src/iModMiddleware/iModDaemon)
endif()

if(IMOD_VIEW)
    # finding packages is managed in the Sub-CMakeLists
	add_subdirectory(src/iModMiddleware/iModView)
endif()

if(IMOD_GRIPPER)
    # finding packages is managed in the Sub-CMakeLists
	add_subdirectory(src/iModGripper)
endif()

if(IMOD_BAXTER)
    # finding packages is managed in the Sub-CMakeLists
		add_subdirectory(src/iModMiddleware/iModBaxter)
endif()

if(IMOD_UTILS)
    # finding packages is managed in the Sub-CMakeLists
	add_subdirectory(src/utils)
endif()

if(IMOD_WII)
	#Compile wiiuse library
	SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${iMod_SOURCE_DIR}/external/wiiuse/cmake")
	SET(CMAKE_CURRENT_SOURCE_DIR ${iMod_SOURCE_DIR}"/external/wiiuse")
	add_subdirectory(external/wiiuse)

	add_subdirectory(src/iModWii)
endif()

