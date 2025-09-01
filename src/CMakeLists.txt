add_library(cascade SHARED cascade.cpp)

target_include_directories(cascade PUBLIC 
../include
../external/entt.hpp)
# ${CMAKE_CURRENT_SOURCE_DIR}/../external/SDL/include/SDL3
# ${CMAKE_CURRENT_SOURCE_DIR}/../external/SDL_image/include/SDL3_image)

# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/SDL ${CMAKE_BINARY_DIR}/external/SDL)
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/SDL_image ${CMAKE_BINARY_DIR}/external/SDL3_image)

target_link_libraries(cascade PRIVATE 
)
# SDL3::SDL3
# SDL3_image::SDL3_image)