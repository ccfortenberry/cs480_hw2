cmake_minimum_required(VERSION 3.13)
project(gamelib)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}../../box2d/include)
add_library(gamelib STATIC
    gamelib.cpp
    gamelib_actor.cpp
    gamelib_actor_component.cpp
    gamelib_audio.cpp
    gamelib_box2d.cpp
    gamelib_command.cpp
    gamelib_context.cpp
    gamelib_font.cpp
    gamelib_graphics.cpp
    gamelib_graphics_component.cpp
    gamelib_input_component.cpp
    gamelib_input_handler.cpp
    gamelib_locator.cpp
    gamelib_object.cpp
    gamelib_physics_component.cpp
    gamelib_random.cpp
    gamelib_story_screen.cpp
    gamelib_world.cpp
    hatchetfish_log.cpp
    hatchetfish_stopwatch.cpp
    )

#if (!APPLE)
target_precompile_headers(gamelib PRIVATE pch.h)
#endif()

install(TARGETS gamelib DESTINATION lib)
#[[install(TARGETS
    gamelib.hpp
    gamelib_actor.hpp
    gamelib_actor_component.hpp
    gamelib_audio.hpp
    gamelib_base.hpp
    gamelib_command.hpp
    gamelib_context.hpp
    gamelib_font.hpp
    gamelib_graphics.hpp
    gamelib_graphics_component.hpp
    gamelib_input_component.hpp
    gamelib_input_handler.hpp
    gamelib_locator.hpp
    gamelib_object.hpp
    gamelib_physics_component.hpp
    gamelib_random.hpp
    gamelib_story_screen.hpp
    gamelib_world.hpp
    hatchetfish.hpp
    hatchetfish_log.hpp
    hatchetfish_stopwatch.hpp
    DESTINATION include)
]]
