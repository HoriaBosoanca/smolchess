cmake_minimum_required(VERSION 3.31)
project(smolchess)

set(CMAKE_CXX_STANDARD 23)

file(GLOB SOURCES CONFIGURE_DEPENDS
        *.cpp
)
add_executable(smolchess ${SOURCES})
set(CMAKE_CXX_FLAGS_RELEASE "-static")
if (MINGW)
    target_compile_options(smolchess PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wsign-conversion
    )
endif()
add_custom_command(
        TARGET smolchess POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:smolchess>/assets
)