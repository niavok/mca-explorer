#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "world.h"
#include "tools.h"

static Space getSpaceAroundPlayer(World *world, int radius);

int main(int argc, char** argv) {

    World world;
    //Space searchSpace = {-1,0,-1,0,256,1};
    Space searchSpace = {-16000,0,-16000,16000,0,16000};
    //Space searchSpace = {-1,0,-1,1,0,1};

    (void) argc;
    (void) argv;
    
    world_open("mca", &world);
    //printf("World open\n");
    
    //world_findBlock(&world, BLOCK_IRON_ORE , searchSpace);
    //world_findBlock(&world, BLOCK_BEDROCK , searchSpace);
    
    int argIndex = 1;
    int toFound = 0;
    
    if(argc > 2) {
        if(strcmp(argv[argIndex], "-r") == 0) {
            //Radius limit
            int radius = atoi(argv[argIndex+1]);
            
            searchSpace =  getSpaceAroundPlayer(&world,radius) ;
            argIndex +=2;
        } else if(strcmp(argv[argIndex], "-c") == 0) {
             toFound = atoi(argv[argIndex+1]);
             argIndex +=2;
        }
    }
    BlockType *blockTypes;
    
    int typeCount = argc - argIndex;
    if(typeCount == 0) {
        blockTypes = smalloc(sizeof(BlockType));
        blockTypes[0] = BLOCK_DIAMOND_ORE;
        typeCount = 1;
    } else {
        blockTypes = smalloc(sizeof(BlockType) * typeCount);
        
        for(int i = 0; i < typeCount; i++) {
            blockTypes[i] = block_fromString(argv[argIndex+i]);
        }
    
    }
    
    if(toFound > 0) {
       int radius = 1;
       int found = 0;
       while(toFound > found && radius < 16000) {
           found = 0;
           printf("======================\n");
           printf("Try with radius %d\n", radius);
           searchSpace = getSpaceAroundPlayer(&world, radius);
           for(int i = 0; i < typeCount; i++) {
              found += world_findBlock(&world, blockTypes[i] , searchSpace); 
           }
           
           radius++;
       }

       printf("%d blocks found\n", found);
    
    } else {
        int found = 0;
        for(int i = 0; i < typeCount; i++) {
          found += world_findBlock(&world, blockTypes[i] , searchSpace); 
        }
        printf("%d blocks found\n", found);
    }

    world_close(&world);

    return 0;
}

static Space getSpaceAroundPlayer(World *world, int radius) {
    double position[3];
    Space searchSpace;
    world_getPlayerPosition(world, position);
    
    searchSpace.minX = position[0]-radius;
    searchSpace.maxX = position[0]+radius;
    searchSpace.minY = position[1]-radius;
    searchSpace.maxY = position[1]+radius;
    searchSpace.minZ = position[2]-radius;
    searchSpace.maxZ = position[2]+radius;
    return searchSpace;
}
