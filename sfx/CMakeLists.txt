project("ComputerWarsSFX")

# TGUI
set(TGUI_BACKEND "SFML_GRAPHICS")
FetchContent_Declare(tgui
	GIT_REPOSITORY https://github.com/CasualYT31/TGUI.git
	GIT_TAG 910a81c
)
FetchContent_MakeAvailable(tgui)

set(${PROJECT_NAME}_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include"
	${ComputerWarsEngine_INCLUDE_DIRS}
	${tgui_INCLUDE_DIRS}
	CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

add_library(${PROJECT_NAME} STATIC
	"source/fonts.cpp"
	"source/renderer.cpp"
	"source/texture.cpp"
	"source/userinput.cpp"
	"source/audio.cpp"
	"source/gui/gui.cpp"
	"source/gui/guiregisterinterface.cpp"
	"source/gui/guiscriptinterface.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC
	${ComputerWarsSFX_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} ComputerWarsEngine tgui)
