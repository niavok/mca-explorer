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
        int compressedSize;
        int size;
        char compression;
        unsigned char *compressedData;
        unsigned char *data;
        FILE* file = NULL;
        
        file = fopen(chunk->path, "rb");
        if(file == NULL) {
            printf("Fail to open %s\n", chunk->path);
            return;
        }
        
        fseek(file, chunk->fileSectorOffset * 4096, SEEK_SET);
        
        fread(&compressedSize, sizeof(int), 1, file);
        fread(&compression, sizeof(char), 1, file);
        
        compressedSize = endian_swap(compressedSize) -1;
        
        if(compression != 2) {
            printf("Bad compression: 0x%x received and 0x02 excepted\n", compression);
        }

        printf("Chunk at %d,%d compressed size: %d\n", chunk->x, chunk->z, compressedSize);


        compressedData = smalloc(sizeof(char) * (compressedSize));
        fread(compressedData, sizeof(char), compressedSize, file);
        
        data = zlib_inflate(compressedData, compressedSize, &size); 

        printf("Chunk at %d,%d inflate result %d -> %d \n", chunk->x, chunk->z, compressedSize, size);
        
        fclose(file);
        
        free(compressedData);
        free(data);
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
