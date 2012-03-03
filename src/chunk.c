#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tools.h"
#include "chunk.h"

void chunk_init(Chunk *chunk, char* path) {
    chunk->path = string_clone(path);
    chunk->loaded = 0;
    chunk->blocks = NULL;
}

void chunk_destroy(Chunk *chunk) {
    chunk_unload(chunk);
    free(chunk->path);
}

void chunk_load(Chunk *chunk) {
    if(chunk->loaded) {
        return;
    }

    chunk->blocks = malloc(sizeof(BlockType) * CHUNK_SIZE);
    memset(chunk->blocks, 0, sizeof(BlockType) * CHUNK_SIZE);
    chunk->loaded = 1;
    
    if(chunk->fileSectorSize >0) {
        /* read the file data*/ 
        int size;
        char compression;
        FILE* file = NULL;
        
        file = fopen(chunk->path, "rb");
        if(file == NULL) {
            printf("Fail to open %s\n", chunk->path);
            return;
        }
        
        fseek(file, chunk->fileSectorOffset * 4096, SEEK_SET);
        
        fread(&size, sizeof(int), 1, file);
        fread(&compression, sizeof(char), 1, file);
        
        if(compression != 2) {
            printf("Bad compression: 0x%x received and 0x02 excepted\n", compression);
        }

        printf("Chunk at %d,%d\n", chunk->x, chunk->z);
        
        fclose(file);
    }
    
}

void chunk_unload(Chunk *chunk) {
    if(chunk->loaded) {
        free(chunk->blocks);
        chunk->loaded = 0;
    }
}

void chunk_findBlock(Chunk *chunk, BlockType blockType) {
    int i;
    if(!chunk->loaded) {
        chunk_load(chunk);
    }
    
    for(i = 0; i < CHUNK_SIZE; i++) {
        if(chunk->blocks[i] == blockType) {
            printf("%s found at %d\n",  block_toString(blockType), i);
        }
    }
}




char *block_toString(BlockType blockType) {
    switch(blockType) {
        case BLOCK_COAL_ORE:
            return "Coal Ore";     
        case BLOCK_DIAMOND_ORE:
            return "Coal Ore";
        default:
            return "Unknown block";
    }
}
