#include <stdlib.h>
#include <stdio.h>

#include "world.h"

int main(int argc, char** argv) {

    World world;
    //Space searchSpace = {-1,0,-1,0,256,1};
    Space searchSpace = {-16,0,-16,0,0,0};

    (void) argc;
    (void) argv;
    
    world_open("mca", &world);
    printf("World open\n");
    
    //world_findBlock(&world, BLOCK_IRON_ORE , searchSpace);
    //world_findBlock(&world, BLOCK_BEDROCK , searchSpace);
    world_findBlock(&world, BLOCK_CRAFTING_TABLE , searchSpace);

    world_close(&world);

    return 0;
}
