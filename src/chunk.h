#ifndef CHUNK_H
#define CHUNK_H

#include "world.h"

void chunk_init(Chunk *chunk, char *path);

void chunk_destroy(Chunk *chunk);

int chunk_findBlock(Chunk *chunk, BlockType blockType, Space space);

void chunk_load(Chunk *chunk);

void chunk_unload(Chunk *chunk);

Space chunk_getSpace(Chunk *);

#endif

