#SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${utils_SOURCE_DIR}/../CMakeModules")
SET(CMAKE_AUTOMOC ON)

include_directories(./OculusRift/LibOVR/Include)
INCLUDE_DIRECTORIES(${OPENSCENEGRAPH_INCLUDE_DIRS})

#get all sourcefiles for utils
file(GLOB imodrift_SRC ./imodrift.cpp ./imodriftnetwork.cpp)
file(GLOB osgviewer_SRC ./OSGViewer/oculusviewer.cpp ./OSGViewer/oculusdevice.cpp ./OSGViewer/oculuseventhandler.cpp ./OSGViewer/oculusviewconfig.cpp)
file(GLOB oculusrift_SRC ./OculusRift/3rdParty/EDID/edid.cpp ./OculusRift/3rdParty/TinyXml/tinyxml2.cpp ./OculusRift/LibOVR/Src/Service/Service_NetSessionCommon.cpp ./OculusRift/LibOVR/Src/Service/Service_NetClient.cpp ./OculusRift/LibOVR/Src/OVR_Stereo.cpp ./OculusRift/LibOVR/Src/Displays/OVR_Linux_SDKWindow.cpp ./OculusRift/LibOVR/Src/Displays/OVR_Display.cpp ./OculusRift/LibOVR/Src/Displays/OVR_Linux_Display.cpp ./OculusRift/LibOVR/Src/Net/OVR_BitStream.cpp ./OculusRift/LibOVR/Src/Net/OVR_Unix_Socket.cpp ./OculusRift/LibOVR/Src/Net/OVR_NetworkPlugin.cpp ./OculusRift/LibOVR/Src/Net/OVR_Socket.cpp ./OculusRift/LibOVR/Src/Net/OVR_Session.cpp ./OculusRift/LibOVR/Src/Net/OVR_PacketizedTCPSocket.cpp ./OculusRift/LibOVR/Src/Net/OVR_RPC1.cpp ./OculusRift/LibOVR/Src/CAPI/CAPI_DistortionRenderer.cpp ./OculusRift/LibOVR/Src/CAPI/GL/CAPI_GL_Util.cpp ./OculusRift/LibOVR/Src/CAPI/GL/CAPI_GL_DistortionRenderer.cpp ./OculusRift/LibOVR/Src/CAPI/GL/CAPI_GL_HSWDisplay.cpp ./OculusRift/LibOVR/Src/CAPI/GL/CAPI_GLE.cpp ./OculusRift/LibOVR/Src/CAPI/CAPI_FrameTimeManager.cpp ./OculusRift/LibOVR/Src/CAPI/CAPI_HMDState.cpp ./OculusRift/LibOVR/Src/CAPI/CAPI_LatencyStatistics.cpp ./OculusRift/LibOVR/Src/CAPI/CAPI_HSWDisplay.cpp ./OculusRift/LibOVR/Src/CAPI/CAPI_HMDRenderState.cpp ./OculusRift/LibOVR/Src/Tracking/Tracking_SensorStateReader.cpp ./OculusRift/LibOVR/Src/Util/Util_LatencyTest2Reader.cpp ./OculusRift/LibOVR/Src/Util/Util_SystemGUI.cpp ./OculusRift/LibOVR/Src/Util/Util_Render_Stereo.cpp ./OculusRift/LibOVR/Src/Util/Util_SystemInfo.cpp ./OculusRift/LibOVR/Src/Util/Util_Interface.cpp ./OculusRift/LibOVR/Src/OVR_CAPI.cpp ./OculusRift/LibOVR/Src/OVR_SerialFormat.cpp ./OculusRift/LibOVR/Src/OVR_JSON.cpp ./OculusRift/LibOVR/Src/OVR_Profile.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_Lockless.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_CRC32.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_String_PathUtil.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_String_FormatUtil.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_DebugHelp.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_RefCount.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_Alg.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_File.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_Timer.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_Log.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_System.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_ThreadsPthread.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_FileFILE.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_String.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_Allocator.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_SysFile.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_ThreadCommandQueue.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_Math.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_SharedMemory.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_Std.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_Atomic.cpp ./OculusRift/LibOVR/Src/Kernel/OVR_UTF8Util.cpp) 

set(imodosg_SRC ${imodrift_SRC}	${osgviewer_SRC} ${oculusrift_SRC})

#build library for rift
add_library(oculusrift SHARED ${imodosg_SRC})

#find Qt
find_package(Qt5Core)
find_package(Qt5Network)
find_package(Qt5Gui)

#Building liboculusrift
target_link_libraries(oculusrift Qt5::Core Qt5::Network Qt5::Gui)
target_link_libraries(oculusrift ${OPENSCENEGRAPH_LIBRARIES} rt GL GLU glfw X11 Xxf86vm Xrandr pthread Xi)

