cmake_minimum_required(VERSION 3.30)
project(compiler)

set(CMAKE_CXX_STANDARD 17)

#add_executable(compiler main.cpp)

file(GLOB_RECURSE LIBS "include/*.h")

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} main.cpp
        ${LIBS}
        ${SOURCES}
)