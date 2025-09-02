cmake_minimum_required(VERSION 3.25.1)

project(Cascade)

add_library(Cascade src/cascade.cpp)

add_subdirectory(external/SDL)
add_subdirectory(external/SDL_image)

target_include_directories(Cascade PUBLIC 
"include"
"external/entt.hpp"
"external/SDL/include/SDL3"
"external/SDL_image/include/SDL3_image")

target_link_libraries(Cascade PRIVATE
SDL3::SDL3
SDL3_image::SDL3_image 
)