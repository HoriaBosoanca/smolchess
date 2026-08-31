cmake_minimum_required(VERSION 3.31)
project(smolchess)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS "-static")
if (NOT WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
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
include(FetchContent)
FetchContent_Declare(
    cutechess
    GIT_REPOSITORY https://github.com/cutechess/cutechess
    GIT_TAG v1.5.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(cutechess)
add_custom_target(playtest
    COMMAND $<TARGET_FILE:cli>
    -engine cmd=$<TARGET_FILE:smolchess> name=Engine1
    -engine cmd=$<TARGET_FILE:smolchess> name=Engine2
    -each proto=uci tc=inf
    -games 1
    DEPENDS smolchess cli
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)