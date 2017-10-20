set(SRC 
	Wii.cpp
	Wii_CInterface.cpp
	EventContainer.cpp
    ${SRC_HEADER}
)

INCLUDE_DIRECTORIES(
    ../../external/wiiuse-master/src/
	../utils/
	${WIIUSE_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

#build library iModWii
add_library(iModWii SHARED ${SRC})

    add_executable(iModWiiUnitTest ./iModWiiUnitTest.cpp)

	#Building libiModWii
	target_link_libraries(iModWii utils ${WIIUSE_LIBRARIES} ${Boost_LIBRARIES})

    #linking library
    target_link_libraries(iModWiiUnitTest iModWii)
