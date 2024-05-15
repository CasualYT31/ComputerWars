# Minimum version required is 3.14 because of FetchContent_MakeAvailable.
cmake_minimum_required (VERSION 3.14)
include (FetchContent)
include (GoogleTest)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
    cmake_policy (SET CMP0141 NEW)
    set (CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif ()

# Determine if this is a debug or release build.
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_compile_definitions(COMPUTER_WARS_DEBUG)
else ()
    add_compile_definitions(COMPUTER_WARS_RELEASE)
endif ()

# Fetch dependencies.

## GoogleTest
FetchContent_Declare (googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.14.0
)
## For Windows: Prevent overriding the parent project's compiler/linker settings.
set (gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable (googletest)
enable_testing ()

# Define targets.
project ("ComputerWars")

# All of the game's code.
set (ComputerWarsLibrary_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}
    # Add dependency include dirs here.
    CACHE INTERNAL "ComputerWarsLibrary: Include Directories" FORCE)
add_library (ComputerWarsLibrary "component/Printing.cpp")
set_property (TARGET ComputerWarsLibrary PROPERTY CXX_STANDARD 20)
target_include_directories (ComputerWarsLibrary PUBLIC ${ComputerWarsLibrary_INCLUDE_DIRS})
# Dependencies go here:
# target_link_libraries (ComputerWarsLibrary )

# The game itself.
add_executable (ComputerWars "main.cpp")
set_property (TARGET ComputerWars PROPERTY CXX_STANDARD 20)
target_include_directories (ComputerWars PRIVATE ${ComputerWarsLibrary_INCLUDE_DIRS})
target_link_libraries (ComputerWars ComputerWarsLibrary)
# Provide access to the assets folder for development purposes. https://stackoverflow.com/a/61244115.
if (UNIX)
    execute_process (COMMAND "${CMAKE_COMMAND}" -E create_symlink "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets")
else () # WINDOWS.
    file (TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/assets" _srcDir)
    file (TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/assets" _dstDir)
    execute_process (COMMAND cmd.exe /c mklink /J "${_dstDir}" "${_srcDir}")
endif ()

# The tests.
add_executable (ComputerWarsTests "component/PrintingTests.cpp")
set_property (TARGET ComputerWarsTests PROPERTY CXX_STANDARD 20)
target_include_directories (ComputerWarsTests PRIVATE ${ComputerWarsLibrary_INCLUDE_DIRS})
target_link_libraries (ComputerWarsTests gtest gtest_main ComputerWarsLibrary)
gtest_discover_tests (ComputerWarsTests) # https://cmake.org/cmake/help/latest/module/GoogleTest.html
