cmake_minimum_required(VERSION 3.20)
project(Solar)

set(CMAKE_SUPPRESS_REGENERATION ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect source files
#The bat must be run again to view newly created or moved elements
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ../Sources/*.cpp)
file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS ../Sources/*.h ../Sources/*.hpp ../Sources/*.inl)
file(GLOB_RECURSE RES_FILES CONFIGURE_DEPENDS ../Resources/*)
#These are in the directoty folder, containing main mostly
file(GLOB LOCAL_SRC CONFIGURE_DEPENDS *.cpp)
file(GLOB LOCAL_HDR CONFIGURE_DEPENDS *.h *.hpp *.inl)

add_executable(Solar ${SRC_FILES} ${LOCAL_SRC})

# Make VS show headers and resources
target_sources(Solar PRIVATE ${HDR_FILES} ${RES_FILES} ${LOCAL_HDR})

# Visual Studio folder tree mirroring real filesystem
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES
    ${SRC_FILES} ${HDR_FILES} ${RES_FILES} ${LOCAL_SRC} ${LOCAL_HDR}
)



# Run Solar exe with Framework as working directory while build stays in Solar
set_property(TARGET Solar PROPERTY 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

set_target_properties(Solar PROPERTIES MSVC_RUNTIME_LIBRARY "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>")

# Make Solar the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Solar)

target_include_directories(Solar PRIVATE
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

target_compile_options(Solar PRIVATE /W3 /sdl)
target_compile_options(Solar PRIVATE
    $<$<CONFIG:Debug>:/Zi>
)
set_property(TARGET Solar PROPERTY VS_GLOBAL_SupportJustMyCodeDebugging "true")

set_target_properties(Solar PROPERTIES
    COMPILE_DEFINITIONS "_DEBUG;_CONSOLE;GLEW_STATIC")

target_compile_options(Solar PRIVATE /permissive-)

target_link_directories(Solar PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../Resources/External/Libs"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Resources/External/Libs/OpenCascade"
)

set_property(TARGET Solar PROPERTY VS_LINK_LIBRARY_DEPENDENCIES "true")

#linker input, first in a semicolon format and then translated to the format CMake expects
set(_EXTRA_LIBS
"glew32s.lib;glfw3.lib;opengl32.lib;freetype.lib"
)

# Split the semicolon string into a CMake list and link them
string(REPLACE ";" ";" _EXTRA_LIBS_LIST "${_EXTRA_LIBS}")
target_link_libraries(Solar PRIVATE ${_EXTRA_LIBS_LIST})

file(GLOB EXTERNAL_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/../Resources/External/dlls/*.dll")

add_custom_command(TARGET Solar POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${EXTERNAL_DLLS}
            $<TARGET_FILE_DIR:Solar>
    COMMENT "Copying external DLLs to output directory (if changed)"
)

set(build_root "${CMAKE_CURRENT_SOURCE_DIR}/build")

set_target_properties(Solar PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${build_root}/$<CONFIG>"
    ARCHIVE_OUTPUT_DIRECTORY "${build_root}/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${build_root}/$<CONFIG>"
)
