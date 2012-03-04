#include <stdlib.h>
#include <stdio.h>

#include "world.h"

int main(int argc, char** argv) {

    World world;
    Space searchSpace = {-15,0,-15,16,256,16};

    (void) argc;
    (void) argv;
    
    world_open("mca", &world);
    printf("World open\n");
    
    world_findBlock(&world, BLOCK_COAL_ORE , searchSpace);    

    world_close(&world);

    return 0;
}
