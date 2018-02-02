#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utils/game_utils.h"
#include "gfx/gfx.h"
#include "map.h"
#include <zconf.h>
#include <stdio.h>


void start_game(block_size_t, block_size_t, int);

int main(int argc, char**argv) {
    /*
    * So that the rand() works "randomly"
    */

    srand((unsigned int) time(NULL));
    if (argc == 1) {
        start_game(25, 15, 35); //standard when no input
        return 0;
    }

    block_size_t width = (block_size_t) atoi(argv[1]);
    block_size_t height = (block_size_t) atoi(argv[2]);
    block_size_t block_size = (block_size_t) atoi(argv[3]);

    start_game(width, height, block_size);

}

void start_game(block_size_t width, block_size_t height, int box_size) {
    map_t *map = new_map(width, height, box_size);

    gfx_open(width*map->box_size, height*map->box_size, "Snake Game");
    map->print(map);

    movement_result_t result = SUCCESS;

    while (true) {
        int is_key_pressed = gfx_event_waiting();
        if (is_key_pressed) {
            int key_pressed = gfx_wait();
            if (key_pressed == 'x') {
                printf("Thank you for playing!\n");
                break;
            } else if (key_pressed == UP ||
                       key_pressed == DOWN ||
                       key_pressed == LEFT ||
                       key_pressed == RIGHT) {

                result = map->move_snake(map, (moving_direction_t) key_pressed);
            }
        } else {
            result = map->move_snake(map, moving_direction);
        }

        map->move_mice(map);

        if (result!= SUCCESS) {
            printf("You died!\n");
            break;
        }

        gfx_clear();

        map->print(map);

        usleep(125000); // Wait for a bit of time
    }

    map->free(map);
}