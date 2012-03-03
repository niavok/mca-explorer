#include <stdlib.h>
#include <stdio.h>

#include "world.h"

int main(int argc, char** argv) {

    World world;

    (void) argc;
    (void) argv;
    
    world_open("mca", &world);

    world_find_diamond(&world, world_getGlobalSpace(&world));    

    world_close(&world);

    return 0;
}
