#get all sourcefiles for iModTracker-Library
file(GLOB iModHaptic_SRC "*.cpp"
	"Lib/*.cpp"
	"camera.cpp"
	"vector3d.cpp"
	"CollisionBody.cpp"
	"CollisionDetection.cpp"
	"BlenderController.cpp"
	"HapticFeedbackController.cpp"
	"visual.cpp"
	"HapticFeedback.cpp"
	"Wii.cpp"
	"Udp.cpp"
	"ObjTransform.cpp"
	"BulletObject.cpp"
	"../interactionMesh/BlenderController.cpp"
	"../interactionMesh/Controller.cpp"
	"../interactionMesh/Pose.cpp"
	"../interactionMesh/PoseContainer.cpp"
	"../interactionMesh/MessageConverter.cpp"
	"../interactionMesh/Timer.cpp"
	"../../external/wiiuse-master/src/wiiuse.c"


)


#build library iModTracker

add_library(iModHaptic SHARED  ${iModHaptic_SRC} )


#add some directorys -> are they realy needed?!
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR}  ${BULLET_INCLUDE_DIRS} ${SDL_INCLUDE_DIR} )

#Building libiModHaptic
target_link_libraries(iModHaptic utils ${MYSQL_CLIENT_LIBS} ${Boost_LIBRARIES}
    ${BULLET_LIBRARIES} ${SDL_LIBRARY} -lGL -lGLU /vol/vrrender/demos/InteractionModels/Blender/src/pywiiuse/wiiuse/lib/libwiiuse.so )

#-lSDL -lGL -lGLU -lSDL_image)


add_executable(iModHapticUnitTest ./iModHapticUnitTest.cpp)
target_link_libraries(iModHapticUnitTest iModHaptic)


#${WIIUSE_LIBRARIES}
#
#${WIIUSE_INCLUDE_DIRS}
