#include <stdio.h>
#include "game_utils.h"
#include <stdlib.h>


// Generates random number between min and max, inclusive.
block_size_t random_number(block_size_t min, block_size_t max)
{
    int range, result, cutoff;

    if (min >= max)
        return min; // only one outcome possible, or invalid parameters
    range = max-min+1;
    cutoff = (RAND_MAX / range) * range;

    // Rejection method, to be statistically unbiased.

    do {
        result = rand();
    } while (result >= cutoff);

    return (block_size_t) (result % range + min);
}

void clear_screen() {

}
