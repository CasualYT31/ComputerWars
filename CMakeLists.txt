# minimum version required is 3.14 becuase of FetchContent_MakeAvailable
cmake_minimum_required (VERSION 3.14)
project ("ComputerWars")
include (FetchContent)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# GoogleTest
FetchContent_Declare(googletest
	# specify the commit you depend on and update it regularly.
	URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# SystemProperties
FetchContent_Declare(systemproperties
	GIT_REPOSITORY https://github.com/CasualYT31/SystemProperties.git
	GIT_TAG 0.1
)
FetchContent_MakeAvailable(systemproperties)

# Boxer
FetchContent_Declare(boxer
	GIT_REPOSITORY https://github.com/aaronmjacobs/Boxer.git
	GIT_TAG master
)
FetchContent_MakeAvailable(boxer)

# SPDLOG
FetchContent_Declare(spdlog
	URL https://github.com/gabime/spdlog/archive/refs/tags/v1.8.5.zip
)
FetchContent_MakeAvailable(spdlog)

# Nlohmann JSON
FetchContent_Declare(json
	GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git
	GIT_TAG v3.9.1
)
FetchContent_MakeAvailable(json)

# SFML
# Note: Linux requires us to manually install packages?
# https://www.sfml-dev.org/tutorials/2.5/compile-with-cmake.php
FetchContent_Declare(sfml
	GIT_REPOSITORY https://github.com/SFML/SFML.git
	GIT_TAG 2.5.1
)
FetchContent_MakeAvailable(sfml)

# TGUI
FetchContent_Declare(tgui
	GIT_REPOSITORY https://github.com/texus/TGUI.git
	GIT_TAG v0.8.7
)
FetchContent_MakeAvailable(tgui)

# AngelScript
FetchContent_Declare(angelscript
	URL https://www.angelcode.com/angelscript/sdk/files/angelscript_2.35.0.zip
	SOURCE_SUBDIR angelscript/projects/cmake
)
FetchContent_MakeAvailable(angelscript)
# to add add-ons, simply add relevant files to below add_executable statement

# source files
add_executable(ComputerWars "main.cpp"
	${angelscript_SOURCE_DIR}/add_on/scriptarray/scriptarray.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptarray/scriptarray.h
	${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring_utils.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring.h
	${angelscript_SOURCE_DIR}/add_on/scriptbuilder/scriptbuilder.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptbuilder/scriptbuilder.h
	engine/logger.cpp
	engine/logger.h
	engine/language.cpp
	engine/language.h
	engine/safejson.cpp
	engine/safejson.h
	engine/file.h
	engine/file.cpp
	engine/gui.h
	engine/gui.cpp
	engine/script.h
	engine/script.cpp
	sfx/audio.cpp
	sfx/audio.h
	sfx/fonts.cpp
	sfx/fonts.h
	sfx/renderer.cpp
	sfx/renderer.h
	sfx/texture.cpp
	sfx/texture.h
	sfx/userinput.cpp
	sfx/userinput.h
	engine/dialogue.h
	engine/dialogue.cpp
	transition/transitions.cpp
	transition/transitions.h
	awe/army.cpp
	awe/army.h
	awe/bank.cpp
	awe/bank.h
	awe/engine.cpp
	awe/engine.h
	awe/map.cpp
	awe/map.h
	awe/spritesheets.cpp
	awe/spritesheets.h
	awe/tile.cpp
	awe/tile.h
	awe/typedef.h
	awe/unit.cpp
	awe/unit.h
	test/test.cpp
	test/test.h
	test/tests.cpp
	test/tests.h)
set_target_properties(ComputerWars PROPERTIES CXX_STANDARD 17)
target_include_directories(ComputerWars PRIVATE ${systeminfo_SOURCE_DIR}
	${angelscript_SOURCE_DIR}/angelscript/include
	${angelscript_SOURCE_DIR}/add_on/scriptarray
	${angelscript_SOURCE_DIR}/add_on/scriptstdstring
	${angelscript_SOURCE_DIR}/add_on/scriptbuilder
	${CMAKE_CURRENT_LIST_DIR})

# libraries
target_link_libraries(ComputerWars
	gtest_main
	SystemProperties
	Boxer
	spdlog
	nlohmann_json::nlohmann_json
	sfml-audio
	sfml-window
	sfml-graphics
	sfml-system
	tgui
	angelscript
)

# copy SFML's openal.dll file depending on build type
IF (WIN32)
	IF (CMAKE_SIZEOF_VOID_P EQUAL 4) # 32-bit
		file(COPY ${sfml_SOURCE_DIR}/extlibs/bin/x86/openal32.dll
			DESTINATION ${CMAKE_BINARY_DIR})
	ELSEIF (CMAKE_SIZEOF_VOID_P EQUAL 8) # 64-bit
		file(COPY ${sfml_SOURCE_DIR}/extlibs/bin/x64/openal32.dll
			DESTINATION ${CMAKE_BINARY_DIR})
	ENDIF ()
ENDIF ()