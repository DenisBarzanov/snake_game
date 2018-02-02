#ifndef SNAKE_SNAKE_H
#define SNAKE_SNAKE_H

typedef enum { // So as to print the enums directly
    UP = 'w',
    DOWN = 's',
    LEFT = 'a',
    RIGHT = 'd'
} moving_direction_t;

typedef enum {
    HEAD,
    TAIL
} body_part_t;

extern moving_direction_t moving_direction;

#endif //SNAKE_SNAKE_H
