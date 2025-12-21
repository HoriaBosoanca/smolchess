cmake_minimum_required(VERSION 3.31)
project(smolchess)

set(CMAKE_CXX_STANDARD 23)

file(GLOB SOURCES CONFIGURE_DEPENDS
        *.cpp
)
add_executable(smolchess ${SOURCES})
