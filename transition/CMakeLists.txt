project("ComputerWarsTransitions")

set(${PROJECT_NAME}_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include"
	${ComputerWarsSFX_INCLUDE_DIRS}
	CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

add_library(${PROJECT_NAME} STATIC "source/transitions.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC
	${ComputerWarsTransitions_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} ComputerWarsSFX)