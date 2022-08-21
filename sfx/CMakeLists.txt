project("ComputerWarsSFX")

# TGUI
FetchContent_Declare(tgui
	GIT_REPOSITORY https://github.com/texus/TGUI.git
	GIT_TAG v0.9.2
)
FetchContent_MakeAvailable(tgui)

# AngelScript
FetchContent_Declare(angelscript
	URL https://www.angelcode.com/angelscript/sdk/files/angelscript_2.35.0.zip
	SOURCE_SUBDIR angelscript/projects/cmake
)
FetchContent_MakeAvailable(angelscript)
# to add add-ons, simply add relevant files to below add_library statement

add_library(${PROJECT_NAME} STATIC
	"script.cpp"
	${angelscript_SOURCE_DIR}/add_on/scriptarray/scriptarray.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring_utils.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptdictionary/scriptdictionary.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptbuilder/scriptbuilder.cpp
	${angelscript_SOURCE_DIR}/add_on/datetime/datetime.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptfile/scriptfilesystem.cpp
	"fonts.cpp"
	"renderer.cpp"
	"texture.cpp"
	"userinput.cpp"
	"audio.cpp"
	"gui.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}
	${ComputerWarsEngine_INCLUDE_DIRS}
	${angelscript_SOURCE_DIR}/angelscript/include
	${angelscript_SOURCE_DIR}/add_on/scriptarray/
	${angelscript_SOURCE_DIR}/add_on/scriptstdstring/
	${angelscript_SOURCE_DIR}/add_on/scriptdictionary/
	${angelscript_SOURCE_DIR}/add_on/scriptbuilder/
	${angelscript_SOURCE_DIR}/add_on/datetime/
	${angelscript_SOURCE_DIR}/add_on/scriptfile/
	${tgui_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} ComputerWarsEngine angelscript tgui)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
	${ComputerWarsEngine_INCLUDE_DIRS}
	${angelscript_INCLUDE_DIRS}
	${tgui_INCLUDE_DIRS}
	CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)