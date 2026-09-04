cmake_minimum_required(VERSION 3.31)
project(smolchess)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS "-static")
file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp src/engine/*.cpp)
add_executable(smolchess ${SOURCES})
target_include_directories(smolchess PRIVATE
    ${CMAKE_SOURCE_DIR}/src/engine
)
set_target_properties(smolchess PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)
if (WIN32)
    set(CUTECHESS_CLI "${CMAKE_SOURCE_DIR}/build/assets/tools/cutechess-1.5.1-win64/cutechess-cli.exe")
else ()
    set(CUTECHESS_CLI "${CMAKE_SOURCE_DIR}/build/assets/tools/Cute_Chess-1.5.1-x86_64/usr/bin/cutechess-cli")
endif ()
add_custom_target(playtest
    COMMAND "${CUTECHESS_CLI}"
    -engine cmd=$<TARGET_FILE:smolchess> name=p1
    -engine cmd=$<TARGET_FILE:smolchess> name=p2
    -each proto=uci tc=inf
#    -openings file=${CMAKE_SOURCE_DIR}/build/assets/8moves_v3.pgn format=pgn order=random
    -games 1
    -pgnout ${CMAKE_SOURCE_DIR}/build/last_game.pgn
    DEPENDS smolchess
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)