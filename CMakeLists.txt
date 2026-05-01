cmake_minimum_required(VERSION 3.28)
project(Relative_Simulator C)

set(CMAKE_C_STANDARD 11)

# fetching Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
raylib
GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
GIT_TAG "master"
GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)

# source files
set(PROJECT_SOURCES
main.c
        terminal.c
        terminal.h
        config.h
        commander.c
        commander.h
        grid.c
        grid.h
        ship.c
        ship.h
        oscilloscope.c
        oscilloscope.h
)

# executables
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# linking Raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# including directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (EMSCRIPTEN)
    # Force the compiler to output an HTML file instead of a blank binary
    set_target_properties(Relative_Simulator PROPERTIES SUFFIX ".html")

    # Emscripten specific web flags and resource packaging
    target_link_options(Relative_Simulator PRIVATE
            "-sUSE_GLFW=3"
            "-sASYNCIFY=1"
            "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/resources@../resources"
    )

endif()
