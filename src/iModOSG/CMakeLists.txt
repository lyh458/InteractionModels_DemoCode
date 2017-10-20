# find header files for including (e.g. for ides like xcode)
file(GLOB SRC_HEADER *.h ../interactionMesh/*.h)

#get all sourcefiles for iModOSG
set(SRC
    	#"GeodeFinder.cpp"
    "BoneFinder.cpp"
		#"LightSourceFinder.cpp"
		#"SkeletonFinder.cpp"
	"VirtualHuman.cpp"
	"LineCreator.cpp"
	"MyInteractionMeshController.cpp"
	"MyInteractionMeshModel.cpp"
	"MyInteractionMeshView.cpp"
	"CaveAndLocalWrapper/CaveAndLocalWrapper.cpp"

	"HelperClassesAndFunctions.cpp"
	"NodeVisitorCallbackRunner.h"
	"VirtualHumanRigTransformHardware.cpp"

	"MyInteractionLineMesh.cpp"
	"MyInteractionLineMeshView.cpp"
	"MyInteractionTriangleMesh.cpp"
	"MyInteractionTriangleMeshView.cpp"
	"MyInteractionTetrahedronMesh.cpp"
	"MyInteractionTetrahedronMeshView.cpp"

    "VirtualHumanNao.cpp"
	"BoneGen.cpp"
	"CylinderCreator.cpp"
        #"CBandNaoRobotCreator.cpp"

	"../interactionMesh/BlenderController.cpp"
	"../interactionMesh/Controller.cpp"
	"../interactionMesh/Pose.cpp"
	"../interactionMesh/PoseContainer.cpp"
	"../interactionMesh/MessageConverter.cpp"
	"../interactionMesh/Timer.cpp"

	"InteractionRoi.cpp"

	${SRC_HEADER}
	)

INCLUDE_DIRECTORIES(
	"../interactionMesh/"
	${Boost_INCLUDE_DIR}
   	${OPENSCENEGRAPH_INCLUDE_DIRS}
	#        ${EIGEN3_INCLUDE_DIR}
	)

#Some compiler flags
#add_definitions(-std=c++0x)


message (STATUS "Project Root Dir is ${PROJECT_SOURCE_DIR} ")
add_definitions(-DiMODOSG_DATA_DIR=\"${PROJECT_SOURCE_DIR}/../data/\" )

if(IMOD_CAVE)

	add_definitions(-DIMOD_CAVE)

	    set(VAR $ENV{XSITE_ROOT})
	    if (VAR)
	        message(STATUS "XSITE_ROOT is set to ${VAR}")
	        set(CMAKE_MODULE_PATH "${VAR}/CMakeModules")
	    else()
	        message(STATUS "XSITE_ROOT not set. Set this variable to the root folder of
	        your xsite package.")
	    endif()
	    find_package(xsite REQUIRED)
            find_package(OpenSceneGraph REQUIRED COMPONENTS osg osgDB osgGA osgParticle osgShadow)

	set(PLUGIN_NAME iModOSG)
	set(PLUGIN_SOURCES
	    ${SRC}

	)

	##################### NOTE #####################
	# except "Find_package"... there is usually no need
	# to change something below this line!

	# you may add you own Find-scripts

	include_directories(${XSITE_INCLUDE_DIR}
	    ${OPENSCENEGRAPH_INCLUDE_DIRS}
	    ${OPTIONAL_INCLUDE_PATHS}
	    )

	add_library(${PLUGIN_NAME} MODULE ${PLUGIN_SOURCES})
	target_link_libraries(${PLUGIN_NAME}
	    ${OPENSCENEGRAPH_LIBRARIES}
	    ${XSITE_LIBRARIES} utils
	    )

	set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")

	if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	    set_target_properties(${PLUGIN_NAME} PROPERTIES LINK_FLAGS "-Wl,-undefined,dynamic_lookup")
	endif()

else()

	include_directories(${OSGWORKS_INCLUDE_DIR})

	set(SRC
		${SRC}
		"main.cpp"
	)

	if(IMOD_RIFT)
		add_definitions(-DIMOD_RIFT)
	endif(IMOD_RIFT)

	#message (STATUS "OSG Version is ${OPENSCENEGRAPH_VERSION} ")
	#if ( OPENSCENEGRAPH_VERSION VERSION_LESS "3.2" )
		#add_definitions(-DOSG_VERSION_LESS_THAN_3_DOT_2 )
	#endif ()

	#Add final binary
	add_executable(iModOSG ${SRC})

       	target_link_libraries(iModOSG utils ${Boost_LIBRARIES} ${OPENSCENEGRAPH_LIBRARIES} ${OSGWORKS_LIBRARIES} )

	if(NOT WIN32 AND NOT APPLE)
            target_link_libraries(iModOSG pthread)
	endif(NOT WIN32 AND NOT APPLE)

	if(WIN32)
		target_link_libraries(iModOSG ws2_32)
	endif(WIN32)

        if(IMOD_RIFT)
			find_package(Qt5Core)
			find_package(Qt5Gui)
			find_package(Qt5Network)

			include_directories($ENV{IMOD_ROOT}/CPP/src/OculusRift/)

			target_link_libraries(iModOSG Qt5::Core Qt5::Network Qt5::Gui)
            target_link_libraries(iModOSG $ENV{IMOD_ROOT}/CPP/lib/liboculusrift.so)
        endif(IMOD_RIFT)

endif(IMOD_CAVE)
