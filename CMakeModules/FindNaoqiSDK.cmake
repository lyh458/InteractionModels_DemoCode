# - Try to find NaoqiSDK
# Once done this will define
#  NAOQISDK_FOUND - System has NaoqiSDK
#  NAOQISDK_INCLUDE_DIRS - The NaoqiSDK include directories
#  NAOQISDK_LIBRARIES - The libraries needed to use NaoqiSDK
#  NAOQISDK_DEFINITIONS - Compiler switches required for using NaoqiSDK
#  NAO_ROOT - Hints

find_path(NAOQISDK_INCLUDE_DIR alvalue/alvalue.h
	HINTS $ENV{NAO_ROOT}/include
	)

#set(LIBS alproxies alaudio albehavior albehaviorinfo albehaviormanager albonjourdiscovery alboxlibrary alcommon alerror alextractor alfile allauncher allogremote almath almathinternal almemoryfastaccess almodelutils almotionrecorder alparammanager alproject alproxies alpythontools alremotecall alresource alserial alsoap althread altools altts alvalue alvalueconvert alvision )
set(LIBS alvalue alcommon alerror almath alproxies )

FOREACH (LIB ${LIBS})
	find_library(${LIB}_LIBRARY 
		NAMES ${LIB} 
		HINTS $ENV{NAO_ROOT}/lib 
	)
list(APPEND NAOQISDK_LIBRARIES ${${LIB}_LIBRARY})
ENDFOREACH()

#set(NAOQISDK_LIBRARIES ${NAOQISDK_LIBRARY} )
set(NAOQISDK_INCLUDE_DIRS ${NAOQISDK_INCLUDE_DIR} )

message(STATUS "NaoqiSDK include dirs")
message(STATUS "   " ${NAOQISDK_INCLUDE_DIRS})
message(STATUS "   " ${NAOQISDK_LIBRARIES})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set NAOQISDK_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(NaoqiSDK  DEFAULT_MSG
	NAOQISDK_LIBRARIES NAOQISDK_INCLUDE_DIR)
mark_as_advanced(NAOQISDK_INCLUDE_DIR NAOQISDK_LIBRARIES )
