# Minimum version required is 3.14 because of FetchContent_MakeAvailable.
cmake_minimum_required (VERSION 3.14)
include (FetchContent)
include (GoogleTest)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
    cmake_policy (SET CMP0141 NEW)
    set (CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif ()

# Always update dependency timestamps when fetching content via the URL method.
if (POLICY CMP0135)
    cmake_policy (SET CMP0135 NEW)
endif ()

# Determine if this is a debug or release build.
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_compile_definitions (COMPUTER_WARS_DEBUG)
else ()
    add_compile_definitions (COMPUTER_WARS_RELEASE)
endif ()

# Calculate how long the absolute source path is. This is used to calculate relative filenames for code files which helps
# declutter the logs. Much thanks to https://stackoverflow.com/a/40947954.
string (LENGTH "${CMAKE_SOURCE_DIR}/" ROOT_SOURCE_PATH_SIZE)
add_compile_definitions (ROOT_SOURCE_PATH_SIZE=${ROOT_SOURCE_PATH_SIZE})

# Define Git branch and commit hash for use with logging.
execute_process (COMMAND git rev-parse --abbrev-ref HEAD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE GIT_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND git rev-parse HEAD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE GIT_COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE)
add_compile_definitions (GIT_BRANCH="${GIT_BRANCH}" GIT_COMMIT="${GIT_COMMIT_HASH}")

# Fetch dependencies.

## nlohmann::json
FetchContent_Declare (json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable (json)

## FMT
FetchContent_Declare (fmt GIT_REPOSITORY https://github.com/fmtlib/fmt GIT_TAG 10.2.1)
FetchContent_MakeAvailable (fmt)

## SPDLOG
set (SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
FetchContent_Declare (spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.14.1)
FetchContent_MakeAvailable (spdlog)

## Boxer
FetchContent_Declare (boxer GIT_REPOSITORY https://github.com/aaronmjacobs/Boxer.git GIT_TAG master)
FetchContent_MakeAvailable (boxer)

## SystemProperties
FetchContent_Declare (systemproperties GIT_REPOSITORY https://github.com/CasualYT31/SystemProperties.git GIT_TAG 0.6)
FetchContent_MakeAvailable (systemproperties)

## Cpptrace
## Don't forget to build with Debug or RelWithDebInfo for symbols and line information.
FetchContent_Declare (cpptrace GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git GIT_TAG v0.7.0)
FetchContent_MakeAvailable (cpptrace)

## Cxxopts
FetchContent_Declare (cxxopts GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git GIT_TAG v3.2.0)
FetchContent_MakeAvailable (cxxopts)

## AngelScript
FetchContent_Declare (angelscript URL https://www.angelcode.com/angelscript/sdk/files/angelscript_2.37.0.zip SOURCE_SUBDIR angelscript/projects/cmake)
FetchContent_MakeAvailable (angelscript)
# Each add-on should be added to this space-separated list.
# The add-on's folder will be included as an include directory, and the source files contained within the folder will be added to the build.
set (ANGELSCRIPT_ADD_ONS scriptbuilder)

## GoogleTest
FetchContent_Declare (googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.14.0
)
### For Windows: Prevent overriding the parent project's compiler/linker settings.
set (gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable (googletest)
enable_testing ()

# Define targets.
project ("ComputerWars")

# Find each AngelScript add-on's include directory and source files.
if (ANGELSCRIPT_ADD_ONS)
    ## Include directories.
    list (TRANSFORM ANGELSCRIPT_ADD_ONS PREPEND "${angelscript_SOURCE_DIR}/add_on/")
    set (ANGELSCRIPT_ADD_ON_INCLUDE_DIRS ${ANGELSCRIPT_ADD_ON_INCLUDE_DIRS} ${ANGELSCRIPT_ADD_ONS})
    ## Sources.
    list (TRANSFORM ANGELSCRIPT_ADD_ONS APPEND "/*.cpp")
    file (GLOB ADD_ON_FILES ${ANGELSCRIPT_ADD_ONS})
    set (ANGELSCRIPT_ADD_ON_SOURCES ${ANGELSCRIPT_ADD_ON_SOURCES} ${ADD_ON_FILES})
    ## Debugging.
    message ("AngelScript add-on include directories:")
    foreach (X ${ANGELSCRIPT_ADD_ON_INCLUDE_DIRS})
        message ("\t${X}")
    endforeach ()
    message ("AngelScript add-on sources:")
    foreach (X ${ANGELSCRIPT_ADD_ON_SOURCES})
        message ("\t${X}")
    endforeach ()
else ()
    set (ANGELSCRIPT_ADD_ON_INCLUDE_DIRS "")
    set (ANGELSCRIPT_ADD_ON_SOURCES "")
    message ("No AngelScript add-ons are being included")
endif ()

# All of the game's code.
set (ComputerWarsLibrary_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}
    ${nlohmann_json_INCLUDE_DIRS}
    ${fmt_INCLUDE_DIRS}
    ${spdlog_INCLUDE_DIRS}
    ${boxer_INCLUDE_DIRS}
    ${SystemProperties_INCLUDE_DIRS}
    ${ANGELSCRIPT_ADD_ON_INCLUDE_DIRS}
    CACHE INTERNAL "ComputerWarsLibrary: Include Directories" FORCE)
add_library (ComputerWarsLibrary ${ANGELSCRIPT_ADD_ON_SOURCES} "mvc/Controller.cpp" "log/Log.cpp" "script/ScriptEngine.cpp")
set_property (TARGET ComputerWarsLibrary PROPERTY CXX_STANDARD 20)
target_include_directories (ComputerWarsLibrary PUBLIC ${ComputerWarsLibrary_INCLUDE_DIRS})
# Dependencies go here:
target_link_libraries (ComputerWarsLibrary PUBLIC nlohmann_json::nlohmann_json fmt spdlog Boxer SystemProperties cpptrace::cpptrace cxxopts::cxxopts angelscript)

# The game itself.
add_executable (ComputerWars "main.cpp")
set_property (TARGET ComputerWars PROPERTY CXX_STANDARD 20)
target_include_directories (ComputerWars PRIVATE ${ComputerWarsLibrary_INCLUDE_DIRS})
target_link_libraries (ComputerWars ComputerWarsLibrary)
# Provide access to the assets folder for development purposes. https://stackoverflow.com/a/61244115.
# And do the same for test files.
if (UNIX)
    execute_process (COMMAND "${CMAKE_COMMAND}" -E create_symlink "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets")
    execute_process (COMMAND "${CMAKE_COMMAND}" -E create_symlink "${CMAKE_SOURCE_DIR}/test-files" "${CMAKE_BINARY_DIR}/test-files")
else () # WINDOWS.
    file (TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/assets" _srcDir)
    file (TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/assets" _dstDir)
    execute_process (COMMAND cmd.exe /c mklink /J "${_dstDir}" "${_srcDir}")
    file (TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/test-files" _srcDir)
    file (TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/test-files" _dstDir)
    execute_process (COMMAND cmd.exe /c mklink /J "${_dstDir}" "${_srcDir}")
endif ()

# The tests.
add_executable (ComputerWarsTests "main_test.cpp" "mvc/MVCTests.cpp")
set_property (TARGET ComputerWarsTests PROPERTY CXX_STANDARD 20)
target_include_directories (ComputerWarsTests PRIVATE ${ComputerWarsLibrary_INCLUDE_DIRS})
target_link_libraries (ComputerWarsTests gtest ComputerWarsLibrary)
gtest_discover_tests (ComputerWarsTests) # https://cmake.org/cmake/help/latest/module/GoogleTest.html
