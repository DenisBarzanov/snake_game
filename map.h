#ifndef SNAKE_MAP_H
#define SNAKE_MAP_H

#include "snake.h"

typedef char block_size_t;

typedef enum {
    WALL,
    SNAKE,
    FREE,
    APPLE,
    MOUSE
} block_type_t;

typedef struct {
    block_size_t column;
    block_size_t row;
} location_t;

typedef struct {
    location_t location;
    block_type_t type;
    void* previous_snake_block; // NULL by default
    void* next_snake_block; // type == SNAKE
} block_t;

typedef enum {
    SELF_CRASH,
    SUCCESS
} movement_result_t;

typedef struct {
    block_size_t width;
    block_size_t height;
    int box_size;
    void (*print) (void*);
    movement_result_t (*move_snake) (void*, moving_direction_t);
    void (*free) (void*);
    void* (*get_tail) (void*);
    void* (*get_head) (void*);
    void (*place_walls_randomly) (void*);
    void (*place_apple_randomly) (void*);
    void (*move_mice) (void*);
    void (*empty_init) (void*);
    /*
     * void* return type is the block's
     * address that is going to be returned
     * void* parameter type stands for the map itself ("self")
     */
    block_t*** blocks;
} map_t;

void empty_init (void*);

map_t* new_map(block_size_t, block_size_t, int);

location_t random_map_location(block_size_t, block_size_t);

void free_map(void*);

void print_map(void*);

movement_result_t move_snake(void*, moving_direction_t);

void place_walls_randomly(void*);
void place_entity_randomly(void*, block_type_t);
void place_apple_randomly(void*);
void place_mice_randomly(void*);

void move_mouse (void*, block_t*);
void move_mice (void*);

void* get_tail(void*);
void* get_head(void*);
void* get_body_part(map_t*, body_part_t);

#endif //SNAKE_MAP_H
