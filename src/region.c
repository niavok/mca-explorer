#include <stdlib.h>
#include <stdio.h>
#include "tools.h"
#include "region.h"
#include "chunk.h"

void region_init(Region *region, char *fileName, char *basePath) {
    char** nameSplit;
    int nameSplitCount;
    char* path = string_cat(basePath, "/");
    region->path = string_cat(path, fileName);
    free(path);
    
    nameSplit = string_split(fileName, '.', &nameSplitCount);
    
    if(nameSplitCount != 4) {
        printf("Bad split of region file name '%s': %d part instead of 4 excepted\n", fileName, nameSplitCount);
        array_free(nameSplit, nameSplitCount);
        return;
    }
    
    region->regionX = atoi(nameSplit[1]);
    region->regionZ = atoi(nameSplit[2]);
    
    region->space.minX = region->regionX * REGION_WIDTH * CHUNK_WIDTH;
    region->space.minZ = region->regionZ * REGION_WIDTH * CHUNK_WIDTH;
    region->space.minY = 0;
    region->space.maxX = region->space.minX + REGION_WIDTH * CHUNK_WIDTH;
    region->space.maxZ = region->space.minZ + REGION_WIDTH * CHUNK_WIDTH;
    region->space.maxY = 256;
    
    
    //printf("X: '%d' ; Z: '%d'\n", region->regionX, region->regionZ);
    
    array_free(nameSplit, nameSplitCount);

    
    region->chunkCount = 0;
    region->loaded = 0;
    region->chunks = NULL;
}



void region_destroy(Region *region) {
    free(region->path);
    region_unload(region);
}

void region_load(Region *region) {
    int i;
    

     if(region->loaded) {
        return;
    }
    region->chunkCount = REGION_WIDTH * REGION_WIDTH;
    region->chunks = smalloc(sizeof(Chunk*) * region->chunkCount);
    
    for(i = 0; i < region->chunkCount; i++) {
        region->chunks[i] = smalloc(sizeof(Chunk));
        chunk_init(region->chunks[i], region->path);
    }
    
    /* read the file header*/ 
    {
        FILE* file = NULL;
        int* locations = NULL;
        int* timestamps = NULL;
        int i;
        
        file = fopen(region->path, "rb");
        if(file == NULL) {
            printf("Fail to open %s\n", region->path);
            return;
        }

        locations = smalloc(sizeof(int) * region->chunkCount);
        timestamps = smalloc(sizeof(int) * region->chunkCount);

        fread(locations, sizeof(int), region->chunkCount, file);
        fread(timestamps, sizeof(int), region->chunkCount, file);
        fclose(file);

        for(i = 0; i < region->chunkCount; i++) {
            unsigned int location = endian_swap(locations[i]);

            region->chunks[i]->fileSectorOffset = (location & 0xFFFFFF00) >>8;
            region->chunks[i]->fileSectorSize = (location & 0x000000FF);
            region->chunks[i]->fileTitmestamp = endian_swap(timestamps[i]);
            region->chunks[i]->x = (i % 32)*16 + region->space.minX;
            region->chunks[i]->z = (i / 32)*16 + region->space.minZ;
        }

        free(locations);        
        free(timestamps);
        
    }    
    
    
    
    region->loaded = 1;
}

void region_unload(Region *region) {
    if(region->loaded) {
        int i;
        for(i = 0; i < region->chunkCount; i++) {
            chunk_destroy(region->chunks[i]);
            free(region->chunks[i]);
        }
        free(region->chunks);
        region->loaded = 0;
    }
}

int region_findBlock(Region *region, BlockType blockType, Space space) {
    int i;
    int found = 0;
    if(!region->loaded) {
        region_load(region);
    }
    
    for(i = 0; i < region->chunkCount; i++) {
        if(space_intersect(chunk_getSpace(region->chunks[i]), space)) { 
            found += chunk_findBlock(region->chunks[i], blockType, space);
        }
    }
    return found;
}


