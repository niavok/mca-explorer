#ifndef WORLD_H
#define WORLD_H

#include "world_constants.h"

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
    int regionCount;
    Region **regions;
    Space space;
};

struct Region {
    int regionX;    
    int regionZ;
    char *path;
    Space space;
    int chunkCount;
    Chunk **chunks;
    char loaded;
};

struct Chunk {
    int x;    
    int z;
    int fileSectorOffset;
    char fileSectorSize;
    int fileTitmestamp;
    BlockType *blocks;
    char *path;
    char loaded;
    
};



void world_open(char *name, World *world);

void world_close(World *world);

void world_findBlock(World *world, BlockType blockType, Space space);

Space world_getGlobalSpace(World *world);

char space_intersect(Space s1, Space s2);

#endif
