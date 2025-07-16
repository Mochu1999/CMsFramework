cmake_minimum_required(VERSION 3.20)
project(CMsFramework)

set(CMAKE_SUPPRESS_REGENERATION ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# Collect all .cpp files recursively in Sources/
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS Sources/*.cpp)

add_executable(CMsFramework ${SRC_FILES})


#debug mode in advance properties. The flag is off but it should be running
set_target_properties(CMsFramework PROPERTIES MSVC_RUNTIME_LIBRARY "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>")

# Make CMsFramework the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CMsFramework)


#C/C++ general
target_include_directories(CMsFramework PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/Stability"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/Deprecated"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/Composites/Solar"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/Composites/MRS"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/Composites/OpenCascade"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/Composites/Telemetry"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/Composites"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/BasicsRendering"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/Core"
	"${CMAKE_CURRENT_SOURCE_DIR}/Resources/External/Includes"
	"${CMAKE_CURRENT_SOURCE_DIR}/Resources/External/Includes/OpenCascade"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources"
	"${CMAKE_CURRENT_SOURCE_DIR}/Resources/External/Includes/OpenCascade"
	)

target_compile_options(CMsFramework PRIVATE /W3 /sdl) # Warning level 3  +  SDL checks
target_compile_options(CMsFramework PRIVATE
    $<$<CONFIG:Debug>:/Zi>
)
set_property(TARGET CMsFramework PROPERTY
    VS_GLOBAL_SupportJustMyCodeDebugging "true")

#Preprocessor definitions
set_target_properties(CMsFramework PROPERTIES
    COMPILE_DEFINITIONS "_DEBUG;_CONSOLE;GLEW_STATIC"
)

#Language conformance
target_compile_options(CMsFramework PRIVATE /permissive-)

#linker general
target_link_directories(CMsFramework PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources/External/Libs"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources/External/Libs/OpenCascade"
)

set_property(TARGET CMsFramework PROPERTY VS_LINK_LIBRARY_DEPENDENCIES "true")


#linker input, first in a semicolon format and then translated to the format CMake expects
set(_EXTRA_LIBS
"glew32s.lib;glfw3.lib;opengl32.lib;freetype.lib;TKBin.lib;TKBinL.lib;TKBinTObj.lib;TKBinXCAF.lib;TKBO.lib;TKBool.lib;TKBRep.lib;TKCAF.lib;TKCDF.lib;TKD3DHost.lib;TKD3DHostTest.lib;TKDCAF.lib;TKDE.lib;TKDECascade.lib;TKDEGLTF.lib;TKDEIGES.lib;TKDEOBJ.lib;TKDEPLY.lib;TKDESTEP.lib;TKDESTL.lib;TKDEVRML.lib;TKDFBrowser.lib;TKDraw.lib;TKernel.lib;TKExpress.lib;TKFeat.lib;TKFillet.lib;TKG2d.lib;TKG3d.lib;TKGeomAlgo.lib;TKGeomBase.lib;TKHLR.lib;TKIVtk.lib;TKIVtkDraw.lib;TKLCAF.lib;TKMath.lib;TKMesh.lib;TKMeshVS.lib;TKMessageModel.lib;TKMessageView.lib;TKOffset.lib;TKOpenGl.lib;TKOpenGles.lib;TKOpenGlesTest.lib;TKOpenGlTest.lib;TKPrim.lib;TKQADraw.lib;TKRWMesh.lib;TKService.lib;TKShapeView.lib;TKShHealing.lib;TKStd.lib;TKStdL.lib;TKTInspector.lib;TKTInspectorAPI.lib;TKTObj.lib;TKTObjDRAW.lib;TKToolsDraw.lib;TKTopAlgo.lib;TKTopTest.lib;TKTreeModel.lib;TKV3d.lib;TKVCAF.lib;TKView.lib;TKViewerTest.lib;TKVInspector.lib;TKXCAF.lib;TKXDEDRAW.lib;TKXMesh.lib;TKXml.lib;TKXmlL.lib;TKXmlTObj.lib;TKXmlXCAF.lib;TKXSBase.lib;TKXSDRAW.lib;TKXSDRAWDE.lib;TKXSDRAWGLTF.lib;TKXSDRAWIGES.lib;TKXSDRAWOBJ.lib;TKXSDRAWPLY.lib;TKXSDRAWSTEP.lib;TKXSDRAWSTL.lib;TKXSDRAWVRML.lib"
)

# Split the semicolon string into a CMake list and link them
string(REPLACE ";" ";" _EXTRA_LIBS_LIST "${_EXTRA_LIBS}")
target_link_libraries(CMsFramework PRIVATE ${_EXTRA_LIBS_LIST})


#Adding dlls automatically
add_custom_command(TARGET CMsFramework POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Resources/External/dlls"
            $<TARGET_FILE_DIR:CMsFramework>
    COMMENT "Copying external DLLs to output directory"
)

# ---- Put ALL build output under Framework/build/<config>/ ----------------
set(build_root "${CMAKE_CURRENT_SOURCE_DIR}/build")

# final .exe/.dll/.lib locations
set_target_properties(CMsFramework PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${build_root}/$<CONFIG>"
    ARCHIVE_OUTPUT_DIRECTORY "${build_root}/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${build_root}/$<CONFIG>"
)

