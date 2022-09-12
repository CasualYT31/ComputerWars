project("ComputerWarsAWE")

add_library(${PROJECT_NAME} STATIC
	"dialogue.cpp"
	"bank.cpp"
	"tile.cpp"
	"unit.cpp"
	"unit_pane.cpp"
	"tile_pane.cpp"
	"army.cpp"
	"army_pane.cpp"
	"map.cpp"
	"spritesheets.cpp"
	"engine.cpp"
	"game.cpp"
	"typedef.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC ${PUBLIC_SOURCE_DIR}
	${ComputerWarsTransitions_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} ComputerWarsTransitions)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
	${ComputerWarsTransitions_INCLUDE_DIRS}
	CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)