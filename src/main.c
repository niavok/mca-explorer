#include <stdlib.h>
#include <stdio.h>

#include "world.h"

int main(int argc, char** argv) {

    World world;
    Chunk *chunk;

    (void) argc;
    (void) argv;
    
    world_open("mca", &world);
    printf("World open\n");
    region_load(world.regions[0]);
    chunk = world.regions[0]->chunks[0];
    printf("Chunk found\n");
    
    chunk_findBlock(chunk, BLOCK_COAL_ORE , world_getGlobalSpace(&world));    

    world_close(&world);

    return 0;
}
