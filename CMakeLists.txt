cmake_minimum_required(VERSION "3.16.3")

project(mine_clone)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "g++-10")

file(GLOB_RECURSE SRC "src/*.h" "src/*.c" "src/*.hpp" "src/*.cpp")

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} GL dl glfw)
include_directories(include)
