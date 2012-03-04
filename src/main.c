#include <stdlib.h>
#include <stdio.h>

#include "world.h"

int main(int argc, char** argv) {

    World world;
    //Space searchSpace = {-1,0,-1,0,256,1};
    //Space searchSpace = {-16000,0,-16000,16000,0,16000};
    Space searchSpace = {-1,0,-1,1,0,1};

    (void) argc;
    (void) argv;
    
    world_open("mca", &world);
    //printf("World open\n");
    
    //world_findBlock(&world, BLOCK_IRON_ORE , searchSpace);
    //world_findBlock(&world, BLOCK_BEDROCK , searchSpace);
    
    if(argc == 2) {
        world_findBlock(&world,  block_fromString(argv[1]) , searchSpace);
    } else {
            world_findBlock(&world, BLOCK_DIAMOND_ORE , searchSpace);
    }

    world_close(&world);

    return 0;
}
