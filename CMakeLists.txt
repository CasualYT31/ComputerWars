# minimum version required is 3.14 because of FetchContent_MakeAvailable
cmake_minimum_required (VERSION 3.14)
project("ComputerWars")
include(FetchContent)
include(GoogleTest)
# I couldn't get CMake to not dump googletest libraries into bin, so I
# decided to just put everything else into bin with it instead
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# engine namespace/library
add_subdirectory("engine")

# sfx namespace/library
add_subdirectory("sfx")

# transition namespace/library
add_subdirectory("transition")

# awe namespace/library
add_subdirectory("awe")

# tests
FetchContent_Declare(googletest
	# specify the commit you depend on and update it regularly.
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG release-1.11.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()
# add compile definition which stores the path to the asset folder to be used for
# testing
add_compile_definitions(
	COMPUTER_WARS_ROOT_TEST_ASSET_FOLDER="${CMAKE_CURRENT_LIST_DIR}/test/assets/")
# ensure that the first three source files REMAIN THE FIRST THREE SOURCE FILES!
# this is because the sink class is a singleton and pretty much everything else
# depends on it in some way, so set it up before carrying out any other tests
add_executable(${PROJECT_NAME}Tests
	"test/sharedfunctions.cpp"
	"test/sinktest.cpp"
	"test/loggertest.cpp"
	"test/jsontest.cpp"
	"test/expandstringtest.cpp"
	"test/languagetest.cpp")
set_target_properties(${PROJECT_NAME}Tests PROPERTIES CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME}Tests PRIVATE
	${ComputerWarsAWE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}Tests gtest gtest_main
	ComputerWarsAWE)
# https://cmake.org/cmake/help/latest/module/GoogleTest.html
gtest_discover_tests(${PROJECT_NAME}Tests)

# application
add_executable(${PROJECT_NAME} "main.cpp")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PRIVATE
	${ComputerWarsAWE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ComputerWarsAWE)