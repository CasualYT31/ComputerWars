project("ComputerWarsAWE")

set(${PROJECT_NAME}_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include"
	${ComputerWarsTransitions_INCLUDE_DIRS}
	CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

add_library(${PROJECT_NAME} STATIC
	"source/bank.cpp"
	"source/tile.cpp"
	"source/unit.cpp"
	"source/animated_tile.cpp"
	"source/animated_unit.cpp"
	"source/army.cpp"
	"source/map/map.cpp"
	"source/map/mapregisterinterface.cpp"
	"source/map/mapmap.cpp"
	"source/map/maparmy.cpp"
	"source/map/mapunit.cpp"
	"source/map/maptile.cpp"
	"source/map/mapgraphics.cpp"
	"source/engine.cpp"
	"source/typedef.cpp"
	"source/mapstrings.cpp"
	
	"source/map/animations/source/daybegin.cpp"
	"source/map/animations/source/tagcos.cpp"
	"source/map/animations/source/tileparticles.cpp"
	"source/map/animations/source/zoom.cpp"
	"source/map/animations/source/labelunit.cpp"
	"source/map/animations/source/capture.cpp"
	"source/map/animations/source/moveunit.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC
	${ComputerWarsAWE_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} ComputerWarsTransitions)
