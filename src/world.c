#include <stdlib.h>
#include <stdio.h>


#include "tools.h"
#include "world.h"


static char *basePath = "/.minecraft/saves/";

void world_open(char *name, World *world) {
    char *rootPath;
    
    rootPath = string_cat(getenv("HOME"), basePath);
    
    world->path = string_clone(name);
    world->path = string_cat(rootPath, name);
    
    free(rootPath);
     
    if(file_exists(world->path)) { 
        printf("World '%s' exists\n", name);
    } else {
        printf("World '%s' not found at %s\n", name, world->path);
    }
        
}

void world_close(World *world) {
    (void) world;
}

void world_find_diamond(World *world, Space space) {
    (void) world;
    (void) space;
}

Space world_getAllSpace(World *world) {
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
