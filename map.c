#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils/game_utils.h"
#include "gfx/gfx.h"
#include "map.h"


bool location_is_in_borded(map_t *, location_t);

map_t *new_map(block_size_t width, block_size_t heigth, int box_size) {
    /*
    * Includes the snake's
    * default coordinates
    */



    /*
     * Allocate memory for the whole map
     */
    block_t ***map_blocks = calloc(heigth, sizeof(block_t **));
    /*
     * Make each of the block wrapper pointers
     * point to an array of block wrappers
     */
    for (block_size_t row = 0; row < heigth; row++) {
        map_blocks[row] = calloc(width, sizeof(block_t *));
    }



    /*
     * Create the map using all of the previous stuff
     */
    map_t *map = malloc(sizeof(map_t));

    map->place_walls_randomly = place_walls_randomly;
    map->place_apple_randomly = place_apple_randomly;
    map->get_tail = get_tail;
    map->get_head = get_head;
    map->print = print_map;
    map->free = free_map;
    map->move_snake = move_snake;
    map->empty_init = empty_init;
    map->blocks = map_blocks;
    map->height = heigth;
    map->width = width;
    map->box_size = box_size;
    map->move_mice = move_mice;


    /*
     * Initialise the map itself with the default values
     */
    map->empty_init(map);


    /*
     * Place the walls
     */
    map->place_walls_randomly(map);


    /*
     * Place the snake in the default coordinates:
     * {0, 0}, {0, 1}, {0, 2}
     * Also, the default moving direction will be DOWN
     * so as not to collide with anything
     */
    map->blocks[0][0]->type = SNAKE;
    /*
     * First one doesn't have a previous one
     */
    map->blocks[0][0]->next_snake_block = map->blocks[0][1];


    map->blocks[0][1]->type = SNAKE;
    map->blocks[0][1]->previous_snake_block = map->blocks[0][0];
    map->blocks[0][1]->next_snake_block = map->blocks[0][2];


    map->blocks[0][2]->type = SNAKE;
    /*
     * The snake's head doesn't have a next one
     */
    map->blocks[0][2]->previous_snake_block = map->blocks[0][1];

    /*
     * Place the apple in a FREE random location of the map
     */
    place_apple_randomly(map);

    /*
     * Place some mice in the map
     */
    place_mice_randomly(map);

    return map;
}

location_t random_map_location(block_size_t width, block_size_t height) {
    location_t apple_location = {
            .row = random_number(0, (block_size_t) (height - 1)), // No memory allocation at width
            .column = random_number(0, (block_size_t) (width - 1))
    };
    return apple_location;
}

void place_apple_randomly(void *map_pointer) {
    place_entity_randomly(map_pointer, APPLE);
}

void free_map(void *map_pointer) {
    map_t *map = map_pointer;
    /*
     * Free all the individual blocks
     */
    for (int row = 0; row < map->height; ++row) {
        for (int column = 0; column < map->width; ++column) {
            free(map->blocks[row][column]);
        }
    }
    /*
     * Free the pointers that point to the arrays of block_t pointers
     */
    for (int row = 0; row < map->height; row++) {
        free(map->blocks[row]);
    }
    /*
     * Free the address of the array of the pointers
     * to the array of block_t pointers
     */
    free(map->blocks);
}

void print_map(void *map_pointer) {
    map_t *map = map_pointer;
    int pixel_x = 0;
    int pixel_y = 0;
    for (int row = 0; row < map->height; ++row) {
        for (int column = 0; column < map->width; ++column) {
            /* More spaces cuz the rows are
             * more dense than the columns
             * The block type actually contains an int*/
            block_type_t block_type = map->blocks[row][column]->type;
            switch (block_type) {
                case APPLE:
                    gfx_color(255, 0, 0); //red
                    break;
                case SNAKE:
                    gfx_color(0, 255, 0); //lime
                    break;
                case WALL:
                    gfx_color(0, 0, 255); //blue
                    break;
                case MOUSE:
                    gfx_color(255, 20, 147); //pink
                    break;
                default://white
                    gfx_color(255, 255, 255);
            }
            if (block_type != FREE) {
                gfx_line(pixel_x, pixel_y, pixel_x + map->box_size, pixel_y + map->box_size);//x inside
                gfx_line(pixel_x + map->box_size, pixel_y, pixel_x, pixel_y + map->box_size);
            }
            gfx_line(pixel_x, pixel_y, pixel_x + map->box_size, pixel_y); //downwards
            gfx_line(pixel_x, pixel_y, pixel_x, pixel_y + map->box_size); //rightwards
            pixel_x += map->box_size;
        }
        pixel_x = 0; // New line after every row
        pixel_y += map->box_size;
    }
    gfx_flush(); // show it all to the screen
}

movement_result_t move_snake(void *map_pointer, moving_direction_t desired_moving_direction) {
    map_t *map = map_pointer;
    block_t *tail_block = map->get_tail(map);
    block_t *head_block = map->get_head(map);

    bool has_eaten_an_apple = false;
    bool has_eaten_a_mouse = false;

    block_t *target_block = NULL;

    moving_direction_t initial_moving_direction = moving_direction;

    bool direction_has_been_reset = false;

    switch (desired_moving_direction) {
        case UP:
            if (moving_direction != DOWN) {
                moving_direction = desired_moving_direction;
            }
            break;
        case DOWN:
            if (moving_direction != UP) {
                moving_direction = desired_moving_direction;
            }
            break;
        case LEFT:
            if (moving_direction != RIGHT) {
                moving_direction = desired_moving_direction;
            }
            break;
        case RIGHT:
            if (moving_direction != LEFT) {
                moving_direction = desired_moving_direction;
            }
            break;
    }

    location_t target_location;

    set_target_location:

    switch (moving_direction) {
        case UP:
            target_location.row = (block_size_t) (head_block->location.row - 1);
            target_location.column = head_block->location.column;
            break;
        case DOWN:
            target_location.row = (block_size_t) (head_block->location.row + 1);
            target_location.column = head_block->location.column;
            break;
        case LEFT:
            target_location.row = head_block->location.row;
            target_location.column = (block_size_t) (head_block->location.column - 1);
            break;
        case RIGHT:
            target_location.row = head_block->location.row;
            target_location.column = (block_size_t) (head_block->location.column + 1);
            break;
    }

    if (!location_is_in_borded(map, target_location)) {
        /*
         * We wanna go on an undefined place
         */
        if (target_location.row < 0) {
            target_location.row = (block_size_t) (map->height - 1);
        } else if (target_location.row > map->height - 1) {
            target_location.row = 0;
        } else if (target_location.column < 0) {
            target_location.column = (block_size_t) (map->width - 1);
        } else if (target_location.column > map->height - 1) {
            target_location.column = 0;
        }
    }

    if (moving_direction != initial_moving_direction &&
        map->blocks[target_location.row][target_location.column]->type == WALL &&
        !direction_has_been_reset) {
        /*
         * If you want to go left or right
         * AND
         * where you want to go is a wall
         * -> reset the moving direction and
         * then try all the other stuff
         */

        moving_direction = initial_moving_direction;
        direction_has_been_reset = true;
        goto set_target_location;

    }


    target_block = map->blocks[target_location.row][target_location.column];


    if (target_block->type == APPLE) {
        has_eaten_an_apple = true;
    } else if(target_block->type == MOUSE) {
        has_eaten_a_mouse = true;
    } else if (target_block->type == SNAKE) {
        return SELF_CRASH;
    } else if (target_block->type == WALL) {
        return SUCCESS;
    }


    target_block->type = SNAKE;

    target_block->previous_snake_block = head_block;
    head_block->next_snake_block = target_block;

    /*
     * Free the tail_block
     * and disconnect it from the snake blocks
     */
    if (has_eaten_an_apple) {

        map->place_apple_randomly(map);


    } else if (has_eaten_a_mouse) {

        int mice_count = random_number(0, 2);
        for (int i = 0; i < mice_count; ++i) {
            place_entity_randomly(map, MOUSE);
        }

    } else {
        tail_block->type = FREE;

        block_t *next_block_to_tail = tail_block->next_snake_block;

        next_block_to_tail->previous_snake_block = NULL;
        /* aka the next one doesn't know the tail anymore */
        tail_block->next_snake_block = NULL;
    }
    return SUCCESS;
}

void *get_tail(void *map_pointer) {
    return get_body_part(map_pointer, TAIL);
}

void *get_head(void *map_pointer) {
    return get_body_part(map_pointer, HEAD);
}

void *get_body_part(map_t *map, body_part_t body_part) {
    for (int row = 0; row < map->height; ++row) {
        for (int column = 0; column < map->width; ++column) {
            block_t *current_block = map->blocks[row][column];
            if (current_block->type == SNAKE) {
                if ((body_part == HEAD &&
                     current_block->next_snake_block == NULL) ||
                    (body_part == TAIL &&
                     current_block->previous_snake_block == NULL)) {
                    /*
                    * If the current_block has no next block or previous block
                    * - aka it is the head or the tail
                    */
                    return current_block;
                }
            }
        }
    }
    return NULL;
}

bool location_is_in_borded(map_t *map, location_t location) {
    if (location.row >= 0 &&
        location.row <= map->height - 1 &&
        location.column >= 0 &&
        location.column <= map->width - 1) {

        return true;
    }
    return false;
}

void place_walls_randomly(void *map_pointer) {
    map_t *map = map_pointer;
    block_size_t walls_amount = random_number((block_size_t) ((map->width + map->height) / 2),
                                              (block_size_t) ((map->width + map->height) - 7));

    for (int i = 0; i < walls_amount; ++i) {
        while (true) {
            location_t wall_location = random_map_location(map->width, map->height);
            if (map->blocks[wall_location.row][wall_location.column]->type == FREE) {
                /*
                 * The apple is about to be placed in a FREE spot
                 */
                map->blocks[wall_location.row][wall_location.column]->type = WALL;
                break;
            }
        }
    }

}

void empty_init(void *map_pointer) {
    map_t *map = map_pointer;
    for (block_size_t row = 0; row < map->height; ++row) {
        for (block_size_t column = 0; column < map->width; ++column) {

            block_t *block = malloc(sizeof(block_t));
            block->location.row = row;
            block->location.column = column;
            block->type = FREE;
            block->previous_snake_block = NULL;
            block->next_snake_block = NULL;

            map->blocks[row][column] = block;

        }
    }
}

void move_mice(void *map_pointer) {
    map_t *map = map_pointer;
    for (int row = 0; row < map->height; ++row) {
        for (int column = 0; column < map->width; ++column) {
            block_t *current_block = map->blocks[row][column];
            if (current_block->type == MOUSE) {
                move_mouse(map, current_block);
            }
        }
    }

}

void move_mouse(void *map_pointer, block_t *mouse_block) {
    map_t *map = map_pointer;
    bool is_going_to_move = random_number(0, 1);
    if (is_going_to_move) {
        moving_direction_t mouse_moving_direction;
        int direction = random_number(1, 4);
        location_t target_location;
        switch (direction) {
            case 1:
                mouse_moving_direction = UP;
                target_location = mouse_block->location;
                target_location.row -= 1;
                break;
            case 2:
                mouse_moving_direction = DOWN;
                target_location = mouse_block->location;
                target_location.row += 1;
                break;
            case 3:
                mouse_moving_direction = LEFT;
                target_location = mouse_block->location;
                target_location.column -= 1;
                break;
            case 4:
                mouse_moving_direction = RIGHT;
                target_location = mouse_block->location;
                target_location.column += 1;
                break;
            default:
                break;
        }
        block_t* target_block;
        switch (mouse_moving_direction) {
            case UP:
                if (location_is_in_borded(map, target_location)) {
                    target_block = map->blocks[mouse_block->location.row - 1][mouse_block->location.column];
                } else {
                    target_block = map->blocks[map->height - 1][mouse_block->location.column];
                    /*
                     * The column is the same, but the row is the downest one
                     */
                }
                break;
            case DOWN:
                if (location_is_in_borded(map, target_location)) {
                    target_block = map->blocks[mouse_block->location.row + 1][mouse_block->location.column];
                } else {
                    target_block = map->blocks[0][mouse_block->location.column];
                    /*
                     * The column is the same, but the row is the uppest one
                     */
                }
                break;
            case LEFT:
                if (location_is_in_borded(map, target_location)) {
                    target_block = map->blocks[mouse_block->location.row][mouse_block->location.column - 1];
                } else {
                    target_block = map->blocks[mouse_block->location.row][map->width - 1];
                    /*
                     * Same row, rightest column
                     */
                }
                break;
            case RIGHT:
                if (location_is_in_borded(map, target_location)) {
                    target_block = map->blocks[mouse_block->location.row][mouse_block->location.column + 1];
                } else {
                    target_block = map->blocks[mouse_block->location.row][0];
                    /*
                     * Same row, leftest column
                     */
                }
                break;
        }
        if (target_block->type == FREE) {
            mouse_block->type = FREE;
            target_block->type = MOUSE;
        }
    }
}

void place_mice_randomly(void *map_pointer) {
    map_t *map = map_pointer;
    int mice_to_be_placed_count = random_number(1, 4);
    for (int i = 0; i < mice_to_be_placed_count; ++i) {
        place_entity_randomly(map, MOUSE);
    }
}

void place_entity_randomly(void *map_pointer, block_type_t entity_type) {
    map_t *map = map_pointer;
    /*
     * Give the entity a random location
     * until it receives one that doesn't
     * collide with the snake
     */
    if (entity_type != APPLE && entity_type != MOUSE) {
        return;
    }

    while (true) {
        location_t apple_location = random_map_location(map->width, map->height);
        if (map->blocks[apple_location.row][apple_location.column]->type == FREE) {
            /*
             * The apple is about to be placed in a FREE spot
             */
            map->blocks[apple_location.row][apple_location.column]->type = entity_type;
            break;
        }
    }
}