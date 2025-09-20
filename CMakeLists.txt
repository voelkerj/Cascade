cmake_minimum_required(VERSION 3.25.1)

project(Cascade)

add_library(Cascade STATIC
${CMAKE_CURRENT_SOURCE_DIR}/src/game.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/inputs.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/system.cpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_image)

target_include_directories(Cascade PUBLIC 
"${CMAKE_CURRENT_SOURCE_DIR}/include"
"${CMAKE_CURRENT_SOURCE_DIR}/external/entt.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/include"
"${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/include/SDL3"
"${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_image/include/SDL3_image")

target_link_libraries(Cascade PUBLIC
SDL3::SDL3
SDL3_image::SDL3_image 
)