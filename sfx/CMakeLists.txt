project("ComputerWarsSFX")

# TGUI
FetchContent_Declare(tgui
	GIT_REPOSITORY https://github.com/texus/TGUI.git
	GIT_TAG v0.8.9
)
FetchContent_MakeAvailable(tgui)

add_library(${PROJECT_NAME} STATIC
	"fonts.cpp"
	"renderer.cpp"
	"texture.cpp"
	"userinput.cpp"
	"audio.cpp"
	"gui.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}
	${ComputerWarsEngine_INCLUDE_DIRS}
	${tgui_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} ComputerWarsEngine tgui)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
	${ComputerWarsEngine_INCLUDE_DIRS}
	${tgui_INCLUDE_DIRS}
	CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)