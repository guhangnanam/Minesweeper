cmake_minimum_required(VERSION 3.24)
project(Project3)

set(CMAKE_CXX_STANDARD 14)

add_executable(Project3 main.cpp cmake-build-debug/Board.h Random.h Random.cpp Buttons.cpp Buttons.h)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR C:/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

include_directories(c:/SFML/include/SFML)
target_link_libraries(Project3 sfml-system sfml-window sfml-graphics sfml-audio)
