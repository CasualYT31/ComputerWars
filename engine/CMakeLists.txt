# https://stackoverflow.com/questions/16398937/cmake-and-finding-other-projects-and-their-dependencies/16404000

# Minimum version required is 3.14 because of FetchContent_MakeAvailable.
cmake_minimum_required(VERSION 3.14)
project("ComputerWarsEngine")
include(FetchContent)

# SystemProperties.
FetchContent_Declare(systemproperties
	GIT_REPOSITORY https://github.com/CasualYT31/SystemProperties.git
	GIT_TAG 0.6
)
FetchContent_MakeAvailable(systemproperties)

# Boxer.
FetchContent_Declare(boxer
	GIT_REPOSITORY https://github.com/aaronmjacobs/Boxer.git
	GIT_TAG master
)
FetchContent_MakeAvailable(boxer)

# SPDLOG.
FetchContent_Declare(spdlog
	URL https://github.com/gabime/spdlog/archive/refs/tags/v1.8.5.zip
)
FetchContent_MakeAvailable(spdlog)

# Nlohmann JSON.
FetchContent_Declare(json
	GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git
	GIT_TAG v3.9.1
)
FetchContent_MakeAvailable(json)

# SFML.
# Note: Linux requires us to manually install packages?
# https://www.sfml-dev.org/tutorials/2.5/compile-with-cmake.php
FetchContent_Declare(sfml
	GIT_REPOSITORY https://github.com/SFML/SFML.git
	GIT_TAG 2.5.1
)
FetchContent_MakeAvailable(sfml)

# AngelScript
FetchContent_Declare(angelscript
	URL https://www.angelcode.com/angelscript/sdk/files/angelscript_2.35.0.zip
	SOURCE_SUBDIR angelscript/projects/cmake
)
FetchContent_MakeAvailable(angelscript)
# To add add-ons, simply add relevant files to below add_library statement.

# AngelScript Docgen.
FetchContent_Declare(docgen
	GIT_REPOSITORY https://github.com/CasualYT31/angelscript-docgen.git
	GIT_TAG master
)
FetchContent_MakeAvailable(docgen)

# Expose public includes to other projects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include"
	${SystemProperties_INCLUDE_DIRS}
	${Boxer_INCLUDE_DIRS}
	${spdlog_INCLUDE_DIRS}
	${nlohmann_json_INCLUDE_DIRS}
	${sfml_INCLUDE_DIRS}
	${angelscript_SOURCE_DIR}/angelscript/include
	CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

add_library(${PROJECT_NAME} STATIC
	"source/logger.cpp"
	"source/safejson.cpp"
	"source/language.cpp"
	"source/file.cpp"
	"source/script.cpp"
	${angelscript_SOURCE_DIR}/add_on/scriptarray/scriptarray.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring_utils.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptdictionary/scriptdictionary.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptbuilder/scriptbuilder.cpp
	${angelscript_SOURCE_DIR}/add_on/datetime/datetime.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptfile/scriptfilesystem.cpp
	${angelscript_SOURCE_DIR}/add_on/scripthelper/scripthelper.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptany/scriptany.cpp
	${docgen_SOURCE_DIR}/source/docgen.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
	${ComputerWarsEngine_INCLUDE_DIRS}
	${angelscript_SOURCE_DIR}/add_on/scriptarray/
	${angelscript_SOURCE_DIR}/add_on/scriptstdstring/
	${angelscript_SOURCE_DIR}/add_on/scriptdictionary/
	${angelscript_SOURCE_DIR}/add_on/scriptbuilder/
	${angelscript_SOURCE_DIR}/add_on/datetime/
	${angelscript_SOURCE_DIR}/add_on/scriptfile/
	${angelscript_SOURCE_DIR}/add_on/scripthelper/
	${angelscript_SOURCE_DIR}/add_on/scriptany/
	${docgen_SOURCE_DIR}/source/)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME}
	SystemProperties
	Boxer
	spdlog
	nlohmann_json::nlohmann_json
	sfml-audio
	sfml-window
	sfml-graphics
	sfml-system
	angelscript)

# Copy SFML's openal.dll file depending on build type.
IF (WIN32)
	IF (CMAKE_SIZEOF_VOID_P EQUAL 4) # 32-bit
		file(COPY ${sfml_SOURCE_DIR}/extlibs/bin/x86/openal32.dll
			DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
	ELSEIF (CMAKE_SIZEOF_VOID_P EQUAL 8) # 64-bit
		file(COPY ${sfml_SOURCE_DIR}/extlibs/bin/x64/openal32.dll
			DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
	ENDIF ()
ENDIF ()