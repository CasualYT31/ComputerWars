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

set(CW_CXX_STANDARD "20" CACHE STRING "The C++ standard to compile Computer Wars (and the TGUI dependency) in")
# Macro generation inputs.
# set(CW_MAX_FIELDS_FOR_GAME_PROPERTY_MACROS "10" CACHE STRING "The maximum number of fields available for GAME_PROPERTY_N macros")
# set(CW_MAX_FIELDS_FOR_POD_TYPE_MACROS "5" CACHE STRING "The maximum number of fields available for POD_N macros")

# engine namespace/library.
add_subdirectory("engine")

# sfx namespace/library.
add_subdirectory("sfx")

# awe namespace/library.
add_subdirectory("awe")

# Tests.
add_subdirectory("test")

# Application.
add_executable(${PROJECT_NAME} "main.cpp")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD ${CW_CXX_STANDARD})
target_include_directories(${PROJECT_NAME} PRIVATE
	${ComputerWarsAWE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ComputerWarsAWE)

# For a second, I thought adding macro generation as a build step would be a good idea, but it's not.
# I think the better approach is to already have the correct macros committed,
# and if a developer wishes to increase the max field counts of either macro type,
# they should invoke the script manually, and commit them.

# Build target used for macro generation.
# https://stackoverflow.com/a/49021383.
# find_package(PythonInterp 3.2 REQUIRED)
# add_custom_target(macro_generation ALL
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/macgen.py --game-properties ${CW_MAX_FIELDS_FOR_GAME_PROPERTY_MACROS} --plain-old-datatypes ${CW_MAX_FIELDS_FOR_POD_TYPE_MACROS}
#	COMMENT "Generating macros")
# add_dependencies(${PROJECT_NAME} macro_generation)
