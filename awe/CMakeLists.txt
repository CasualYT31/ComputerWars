project("ComputerWarsAWE")

add_library(${PROJECT_NAME} STATIC
	"dialogue.cpp"
	"bank.cpp"
	"tile.cpp"
	"unit.cpp"
	"army.cpp"
	"map.cpp"
	"spritesheets.cpp"
	"engine.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC ${PUBLIC_SOURCE_DIR}
	${ComputerWarsSFX_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} ComputerWarsSFX)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
	${ComputerWarsSFX_INCLUDE_DIRS}
	CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)