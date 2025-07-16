cmake_minimum_required(VERSION 3.20)
project(CMsFramework)

set(CMAKE_SUPPRESS_REGENERATION ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all .cpp files recursively in Sources/
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS Sources/*.cpp)

add_executable(CMsFramework ${SRC_FILES})

# Make it the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CMsFramework)

