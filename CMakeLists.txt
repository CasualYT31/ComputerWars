# Minimum version required is 3.14 because of FetchContent_MakeAvailable.
cmake_minimum_required (VERSION 3.14)
project("ComputerWars")
include(FetchContent)
include(GoogleTest)
# I couldn't get CMake to not dump googletest libraries into bin, so I
# decided to just put everything else into bin with it instead.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Determine if this is a debug or release build.
if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
	add_compile_definitions(COMPUTER_WARS_DEBUG)
else()
	add_compile_definitions(COMPUTER_WARS_RELEASE)
endif()

# Generate script interface documentation if set to ON.
option(CW_GENERATE_SCRIPT_DOC "Generates the game engine's script interface documentation" OFF)
if(CW_GENERATE_SCRIPT_DOC)
	add_compile_definitions(AS_GENERATE_DOCUMENTATION=1)
else()
	add_compile_definitions(AS_GENERATE_DOCUMENTATION=0)
endif()

# engine namespace/library.
add_subdirectory("engine")

# sfx namespace/library.
add_subdirectory("sfx")

# transition namespace/library.
add_subdirectory("transition")

# awe namespace/library.
add_subdirectory("awe")

# Tests.
add_subdirectory("test")

# Application.
add_executable(${PROJECT_NAME} "main.cpp")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PRIVATE
	${ComputerWarsAWE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ComputerWarsAWE)