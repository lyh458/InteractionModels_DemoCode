# - Try to find OSGWORKS
# Once done this will define
#  OSGWORKS_FOUND - System has OSGWORKS 
#  OSGWORKS_INCLUDE_DIRS - The OSGWORKS include directories
#  OSGWORKS_LIBRARIES - The libraries needed to use OSGWORKS
#  OSGWORKS_ROOT - Hints

find_path(OSGWORKS_INCLUDE_DIR osgwControls/Export.h
	HINTS $ENV{OSGWORKS_ROOT}/include
	)

set(LIBS osgwControls osgwMx osgwQuery osgwTools)

FOREACH (LIB ${LIBS})
	find_library(${LIB}_LIBRARY 
		NAMES ${LIB} 
		HINTS $ENV{OSGWORKS_ROOT}/lib 
	)
list(APPEND OSGWORKS_LIBRARIES ${${LIB}_LIBRARY})
ENDFOREACH()

#set(OSGWORKS_LIBRARIES ${OSGWORKS_LIBRARY} )
set(OSGWORKS_INCLUDE_DIRS ${OSGWORKS_INCLUDE_DIR} )

message(STATUS "OSGWORKS include dirs")
message(STATUS "   " ${OSGWORKS_INCLUDE_DIRS})
message(STATUS "   " ${OSGWORKS_LIBRARIES})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set OSGWORKS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(OSGWORKS  DEFAULT_MSG
	OSGWORKS_LIBRARIES OSGWORKS_INCLUDE_DIR)
mark_as_advanced(OSGWORKS_INCLUDE_DIR OSGWORKS_LIBRARIES )
