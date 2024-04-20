project("ComputerWarsSFX")

# TGUI
set(TGUI_BACKEND "SFML_GRAPHICS")
set(TGUI_CXX_STANDARD ${CW_CXX_STANDARD})
FetchContent_Declare(tgui
	# Don't forget to update documentation links in guiregisterinterface.cpp
	# when updating the version of TGUI.
	GIT_REPOSITORY https://github.com/CasualYT31/TGUI.git
	GIT_TAG 413f641a
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
	"source/gui/guiscriptinterface.cpp"
	"source/gui/guisignalhandlers.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC
	${ComputerWarsSFX_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD ${CW_CXX_STANDARD})
target_link_libraries(${PROJECT_NAME} ComputerWarsEngine tgui)
