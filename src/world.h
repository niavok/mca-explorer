#ifndef WORLD_H
#define WORLD_H

typedef struct World World;
typedef struct Region Region;
typedef struct Chunk Chunk;
typedef struct Space Space;

struct Space {
    int minX;    
    int minY;
    int minZ;
    int maxX;
    int maxY;
    int maxZ;
};

struct World {
    char* name;
    char* path;
    char exist;
    Region **regions;
    Space globalSpace;
};

struct Region {
    int x;    
    int z;
};

struct Chunk {
    int x;    
    int z;
};



void world_open(char *name, World *world);

void world_close(World *world);

void world_find_diamond(World *world, Space space);

Space world_getGlobalSpace(World *world);

#endif
