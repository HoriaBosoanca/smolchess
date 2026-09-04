cmake_minimum_required(VERSION 3.31)
project(smolchess)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS "-static")
# https://github.com/cutechess/cutechess/releases/tag/v1.5.1
    # in MSYS2 MINGW64: pacman -S mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-svg mingw-w64-x86_64-qt6-5compat
#    set(CMAKE_PREFIX_PATH "C:/msys64/mingw64" ${CMAKE_PREFIX_PATH})
    # add C:/msys64/mingw64/bin to path
#    set(ENV{PATH} "C:/msys64/mingw64/bin;$ENV{PATH}")
if (NOT WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    # sudo apt install qt6-base-dev qt6-svg-dev qt6-5compat-dev
endif ()
file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(smolchess ${SOURCES})
if (MINGW)
    target_compile_options(smolchess PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
    )
endif()
set_target_properties(smolchess PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)
#include(FetchContent)
#FetchContent_Declare(
#    cutechess
#    GIT_REPOSITORY https://github.com/cutechess/cutechess
#    GIT_TAG v1.5.1
#    GIT_SHALLOW TRUE
#)
#FetchContent_MakeAvailable(cutechess)
if (WIN32)
    set(CUTECHESS_CLI "C:/Program Files (x86)/Cute Chess/cutechess-cli.exe")
else ()
    set(CUTECHESS_CLI "C:/Program Files (x86)/Cute Chess/cutechess-cli.exe")
endif ()
add_custom_target(playtest
    COMMAND "${CUTECHESS_CLI}"
    -engine cmd=$<TARGET_FILE:smolchess> name=Engine1
    -engine cmd=$<TARGET_FILE:smolchess> name=Engine2
    -each proto=uci tc=inf
    -games 1
    DEPENDS smolchess
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)