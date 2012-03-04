#ifndef CHUNK_H
#define CHUNK_H

#include "world.h"

void chunk_init(Chunk *chunk, char *path);

void chunk_destroy(Chunk *chunk);

void chunk_findBlock(Chunk *chunk, BlockType blockType);

void chunk_load(Chunk *chunk);

void chunk_unload(Chunk *chunk);

Space chunk_getSpace(Chunk *);

char *block_toString(BlockType blockType);

#endif

