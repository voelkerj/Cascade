cmake_minimum_required(VERSION 3.25.1)

project(Cascade)

add_library(Cascade src/cascade.cpp)

target_include_directories(Cascade PUBLIC 
include
external/entt.hpp)

target_link_libraries(Cascade PRIVATE 
)