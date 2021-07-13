project("ComputerWars")
include(FetchContent)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# engine namespace/library
add_subdirectory("engine")

# sfx namespace/library
add_subdirectory("sfx")

# awe namespace/library
add_subdirectory("awe")

# tests
enable_testing()
add_subdirectory("test")

# source files
add_executable(${PROJECT_NAME} "main.cpp")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PRIVATE
	${ComputerWarsAWE_INCLUDE_DIRS})

# libraries
target_link_libraries(${PROJECT_NAME} ComputerWarsAWE)