cmake_minimum_required(VERSION 3.20)
project(CMsFramework)

set(CMAKE_SUPPRESS_REGENERATION ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect source files
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ../Sources/*.cpp)
file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS ../Sources/*.h ../Sources/*.hpp ../Sources/*.inl)
file(GLOB_RECURSE RES_FILES CONFIGURE_DEPENDS ../Resources/*)
#These are in the directoty folder, containing main mostly
file(GLOB LOCAL_SRC CONFIGURE_DEPENDS *.cpp)
file(GLOB LOCAL_HDR CONFIGURE_DEPENDS *.h *.hpp *.inl)

add_executable(CMsFramework ${SRC_FILES} ${LOCAL_SRC})

# Make VS show headers and resources
target_sources(CMsFramework PRIVATE ${HDR_FILES} ${RES_FILES} ${LOCAL_HDR})

# Visual Studio folder tree mirroring real filesystem
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES
    ${SRC_FILES} ${HDR_FILES} ${RES_FILES} ${LOCAL_SRC} ${LOCAL_HDR}
)



# Run CMsFramework exe with Framework as working directory while build stays in CMsFramework
set_property(TARGET CMsFramework PROPERTY 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

set_target_properties(CMsFramework PROPERTIES MSVC_RUNTIME_LIBRARY "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>")

# Make CMsFramework the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CMsFramework)

target_include_directories(CMsFramework PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/../Sources/Stability"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Sources/Deprecated"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Sources/Composites/Solar"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Sources/Composites/Autopilot"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Sources/Composites/OpenCascade"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Sources/Composites/Telemetry"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Sources/Composites"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Sources/BasicsRendering"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Sources/Core"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Resources/External/Includes"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Resources/External/Includes/OpenCascade"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Sources"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Resources/External/Includes/OpenCascade"
)

target_compile_options(CMsFramework PRIVATE /W3 /sdl)
target_compile_options(CMsFramework PRIVATE
    $<$<CONFIG:Debug>:/Zi>
)
set_property(TARGET CMsFramework PROPERTY VS_GLOBAL_SupportJustMyCodeDebugging "true")

set_target_properties(CMsFramework PROPERTIES
    COMPILE_DEFINITIONS "_DEBUG;_CONSOLE;GLEW_STATIC")

target_compile_options(CMsFramework PRIVATE /permissive-)

target_link_directories(CMsFramework PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../Resources/External/Libs"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Resources/External/Libs/OpenCascade"
)

set_property(TARGET CMsFramework PROPERTY VS_LINK_LIBRARY_DEPENDENCIES "true")

#linker input, first in a semicolon format and then translated to the format CMake expects
set(_EXTRA_LIBS
"glew32s.lib;glfw3.lib;opengl32.lib;freetype.lib;TKBin.lib;TKBinL.lib;TKBinTObj.lib;TKBinXCAF.lib;TKBO.lib;TKBool.lib;TKBRep.lib;TKCAF.lib;TKCDF.lib;TKD3DHost.lib;TKD3DHostTest.lib;TKDCAF.lib;TKDE.lib;TKDECascade.lib;TKDEGLTF.lib;TKDEIGES.lib;TKDEOBJ.lib;TKDEPLY.lib;TKDESTEP.lib;TKDESTL.lib;TKDEVRML.lib;TKDFBrowser.lib;TKDraw.lib;TKernel.lib;TKExpress.lib;TKFeat.lib;TKFillet.lib;TKG2d.lib;TKG3d.lib;TKGeomAlgo.lib;TKGeomBase.lib;TKHLR.lib;TKIVtk.lib;TKIVtkDraw.lib;TKLCAF.lib;TKMath.lib;TKMesh.lib;TKMeshVS.lib;TKMessageModel.lib;TKMessageView.lib;TKOffset.lib;TKOpenGl.lib;TKOpenGles.lib;TKOpenGlesTest.lib;TKOpenGlTest.lib;TKPrim.lib;TKQADraw.lib;TKRWMesh.lib;TKService.lib;TKShapeView.lib;TKShHealing.lib;TKStd.lib;TKStdL.lib;TKTInspector.lib;TKTInspectorAPI.lib;TKTObj.lib;TKTObjDRAW.lib;TKToolsDraw.lib;TKTopAlgo.lib;TKTopTest.lib;TKTreeModel.lib;TKV3d.lib;TKVCAF.lib;TKView.lib;TKViewerTest.lib;TKVInspector.lib;TKXCAF.lib;TKXDEDRAW.lib;TKXMesh.lib;TKXml.lib;TKXmlL.lib;TKXmlTObj.lib;TKXmlXCAF.lib;TKXSBase.lib;TKXSDRAW.lib;TKXSDRAWDE.lib;TKXSDRAWGLTF.lib;TKXSDRAWIGES.lib;TKXSDRAWOBJ.lib;TKXSDRAWPLY.lib;TKXSDRAWSTEP.lib;TKXSDRAWSTL.lib;TKXSDRAWVRML.lib"
)

# Split the semicolon string into a CMake list and link them
string(REPLACE ";" ";" _EXTRA_LIBS_LIST "${_EXTRA_LIBS}")
target_link_libraries(CMsFramework PRIVATE ${_EXTRA_LIBS_LIST})

file(GLOB EXTERNAL_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/../Resources/External/dlls/*.dll")

add_custom_command(TARGET CMsFramework POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${EXTERNAL_DLLS}
            $<TARGET_FILE_DIR:CMsFramework>
    COMMENT "Copying external DLLs to output directory (if changed)"
)

set(build_root "${CMAKE_CURRENT_SOURCE_DIR}/build")

set_target_properties(CMsFramework PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${build_root}/$<CONFIG>"
    ARCHIVE_OUTPUT_DIRECTORY "${build_root}/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${build_root}/$<CONFIG>"
)
