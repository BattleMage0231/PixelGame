cmake_minimum_required(VERSION 3.5)

project(pixelgame)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(game 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/enemy.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/map.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/player.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/static.cc
)

target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(game SDL2::Main SDL2::Image SDL2::TTF glm::glm-header-only)
