# minimum version required is 3.14 becuase of FetchContent_MakeAvailable
cmake_minimum_required (VERSION 3.14)
project ("ComputerWarsTests")
include (FetchContent)
include (GoogleTest)
# I couldn't get CMake to not dump googletest libraries into bin, so I
# decided to just put the test app into bin instead
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# GoogleTest
FetchContent_Declare(googletest
	# specify the commit you depend on and update it regularly.
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG release-1.11.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_executable(TestApp
	engine/sinktest.cpp)
set_target_properties(TestApp PROPERTIES CXX_STANDARD 17)
target_link_libraries(TestApp gtest gtest_main)
# https://cmake.org/cmake/help/latest/module/GoogleTest.html
gtest_add_tests(TARGET TestApp)