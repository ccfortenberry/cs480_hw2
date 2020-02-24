cmake_minimum_required(VERSION 3.13)
project(simplegame)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${gamelib_SOURCE_DIR}/../gamelib)

add_executable(simplegame
    main.cpp
    DungeonActorComponent.cpp
    NewtonPhysicsComponent.cpp
    Game.cpp
    )
target_link_libraries(simplegame gamelib)

set(GCC_EXPECTED_VERSION 9.0.0)
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS GCC_EXPECTED_VERSION)
    message("Using stdc++fs")
    target_link_libraries(simplegame stdc++fs)
endif()

find_library(SDL2_LIB NAMES SDL2)
find_library(SDL2_IMAGE_LIB NAMES SDL2_image)
find_library(SDL2_MIXER_LIB NAMES SDL2_mixer)
find_library(SDL2_TTF_LIB NAMES SDL2_ttf)
find_library(CZMQ_LIB NAMES czmq)
find_library(BOX2D_LIB NAMES Box2D box2d PATHS ../../box2d/build/src)

target_link_libraries(
    ${PROJECT_NAME}
    ${SDL2_LIB}
    ${SDL2_IMAGE_LIB}
    ${SDL2_MIXER_LIB}
    ${SDL2_TTF_LIB}
    ${CZMQ_LIB}
    ${BOX2D_LIB})

install(TARGETS simplegame DESTINATION bin)

