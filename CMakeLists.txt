cmake_minimum_required(VERSION 3.9)
project(snake C)

set(CMAKE_C_STANDARD 11)

add_executable(snake game.c utils/game_utils.c utils/game_utils.h map.c map.h snake.c snake.h gfx/gfx.c gfx/gfx.h)
target_link_libraries(snake X11)
