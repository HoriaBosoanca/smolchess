cmake_minimum_required(VERSION 3.31)
project(smolchess)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS "-static")

file(GLOB SOURCES CONFIGURE_DEPENDS *.cpp)
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