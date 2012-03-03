#include <stdlib.h>
#include <stdio.h>

#include "tools.h"
#include "world.h"


static char *basePath = "/.minecraft/saves/";

void world_open(char *name, World *world) {
    char *rootPath;
    char * regionPath;
    char **regionFileList;
    int i;
    
    rootPath = string_cat(getenv("HOME"), basePath);
    
    world->name = string_clone(name);
    world->path = string_cat(rootPath, name);
    
    free(rootPath);
     
    if(!file_exists(world->path)) { 
        printf("World '%s' not found at %s\n", name, world->path);
        return;
    }
    
    regionPath = string_cat(world->path, "/region");
    regionFileList = file_listDir(regionPath, 1, &(world->regionCount));
    free(regionPath);
    
    
    /* Analyse regions*/
    
    world->regions = smalloc(sizeof(Region) * world->regionCount);
    
    for(i = 0; i < world->regionCount; i++) {
        printf("Region '%s' found\n", regionFileList[i]);
        world->regions[i] = smalloc(sizeof(Region));
        region_init(world->regions[i], regionFileList[i], regionPath);
    }
    
    array_free(regionFileList, world->regionCount);
}

void world_close(World *world) {
    int i;
    free(world->name);
    free(world->path);
    
    for(i = 0; i < world->regionCount; i++) {
        free(world->regions[i]);
    }
    free(world->regions);
    world->regions = NULL;
    world->regionCount = 0;
    
    world->name = 0;
    world->path = 0;
}

void world_find_diamond(World *world, Space space) {
    (void) world;
    (void) space;
}

Space world_getGlobalSpace(World *world) {
    Space space;
    
        (void) world;
    
    space.minX = 0;
    space.minY = 0;
    space.minZ = 0;
    space.maxX = 0;
    space.maxY = 0;
    space.maxZ = 0;
    return space;
}
