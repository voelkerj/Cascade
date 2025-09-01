cmake_minimum_required(VERSION 3.25.1)

project(cascade)

add_library(cascade SHARED src/cascade.cpp)

target_include_directories(cascade PUBLIC 
include
external/entt.hpp)


target_link_libraries(cascade PRIVATE 
)