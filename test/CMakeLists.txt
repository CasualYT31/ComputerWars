# minimum version required is 3.14 because of FetchContent_MakeAvailable
cmake_minimum_required (VERSION 3.14)
project("ComputerWarsTests")
include(FetchContent)
include(GoogleTest)

# first define options for full testing definitions
# the following options are used to select which tests to build and execute; some
# tests are lengthy and the user may wish to skip them by deselecting them
option(CW_FULL_SINK_TESTING "Includes hardware spec in the log when testing" OFF)
option(CW_FULL_AUDIO_TESTING "Includes live audio tests" OFF)
option(CW_FULL_RENDERER_TESTING "Includes renderer tests involving opening render \
windows" OFF)
option(CW_FULL_DRAWABLE_TESTING "Includes calculateDelta() and accumulateDelta() \
tests" OFF)
option(CW_FULL_TEXTURE_TESTING "Includes live sprite tests" OFF)
option(CW_FULL_USERINPUT_TESTING "Includes mouse position tests as well as a \
testing environment in which to test user input" OFF)
option(CW_FULL_TRANSITION_TESTING "Includes transitions tests" OFF)
option(CW_FULL_GUI_TESTING "Includes GUI testing environment" ON)

# GoogleTest
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
	COMPUTER_WARS_ROOT_TEST_ASSET_FOLDER="${CMAKE_CURRENT_LIST_DIR}/assets/"
)
if(CW_FULL_SINK_TESTING)
	add_compile_definitions(COMPUTER_WARS_FULL_SINK_TESTING)
endif()
if(CW_FULL_AUDIO_TESTING)
	add_compile_definitions(COMPUTER_WARS_FULL_AUDIO_TESTING)
endif()
if(CW_FULL_RENDERER_TESTING)
	add_compile_definitions(COMPUTER_WARS_FULL_RENDERER_TESTING)
endif()
if(CW_FULL_DRAWABLE_TESTING)
	add_compile_definitions(COMPUTER_WARS_FULL_DRAWABLE_TESTING)
endif()
if(CW_FULL_TEXTURE_TESTING)
	add_compile_definitions(COMPUTER_WARS_FULL_TEXTURE_TESTING)
endif()
if(CW_FULL_USERINPUT_TESTING)
	add_compile_definitions(COMPUTER_WARS_FULL_USERINPUT_TESTING)
endif()
if(CW_FULL_TRANSITION_TESTING)
	add_compile_definitions(COMPUTER_WARS_FULL_TRANSITION_TESTING)
endif()
if(CW_FULL_GUI_TESTING)
	add_compile_definitions(COMPUTER_WARS_FULL_GUI_TESTING)
endif()

# ensure that the first three source files REMAIN THE FIRST THREE SOURCE FILES!
# this is because the sink class is a singleton and pretty much everything else
# depends on it in some way, so set it up before carrying out any other tests
add_executable(${PROJECT_NAME}
	"sharedfunctions.cpp"
	"sinktest.cpp"
	"loggertest.cpp"
	"jsontest.cpp"
	"expandstringtest.cpp"
	"languagetest.cpp"
	"fontstest.cpp"
	"audiotest.cpp"
	"filetest.cpp"
	"renderertest.cpp"
	"drawabletest.cpp"
	"texturetest.cpp"
	"joysticktest.cpp"
	"converttest.cpp"
	"userinputtest.cpp"
	"transitiontest.cpp"
	"scripttest.cpp"
	"guitest.cpp"
	"banktest.cpp"
	"tiletest.cpp"
	"unittest.cpp"
	"armytest.cpp"
	"maptest.cpp")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PRIVATE ${ComputerWarsAWE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} gtest gtest_main ComputerWarsAWE)
# https://cmake.org/cmake/help/latest/module/GoogleTest.html
gtest_discover_tests(${PROJECT_NAME})