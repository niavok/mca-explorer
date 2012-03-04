#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tools.h"
#include "chunk.h"

#include "ntb.h"

void chunk_init(Chunk *chunk, char* path) {
    chunk->path = string_clone(path);
    chunk->loaded = 0;
    chunk->rootTag = NULL;
    chunk->sections = NULL;
    chunk->sectionCount = 0;
}

void chunk_destroy(Chunk *chunk) {
    chunk_unload(chunk);
    free(chunk->path);
}

void chunk_load(Chunk *chunk) {
    if(chunk->loaded) {
        return;
    }

    chunk->rootTag = NULL;
    chunk->sections = NULL;
    chunk->sectionCount = 0;
    
    if(chunk->fileSectorSize >0) {
        /* read the file data*/ 
        int compressedSize;
        int size;
	int usedSize;
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
        
        /*DEBUG WRITE*/
        /*
        { 
            FILE* file = NULL;
            char * path = malloc(sizeof(char) * 100);
            sprintf(path, "tmp/plop-%d-%d.ntb", chunk->x, chunk->z);
            file = fopen(path, "wb");
            fwrite(data, sizeof(char), size, file);
            free(path);
            fclose(file);            
        }*/
        
        chunk->rootTag = ntb_parseData(data, size, &usedSize);

        ntb_print(chunk->rootTag, 0);        

        printf("Load level tag\n");
        Tag * levelTag = ntb_getTagByName(chunk->rootTag, "Level");
        printf("Load section tag\n");
        Tag * sectionTag = ntb_getTagByName(levelTag, "Sections");
        printf("Loaded\n");
        
        struct TagListPayload *sectionTagPayload = sectionTag->payload;
        
        
        chunk->sectionCount = sectionTagPayload->length;        
        chunk->sections = sectionTagPayload->payload;
        
        printf("Chunk loaded\n");
        

        
        fclose(file);
        
        free(compressedData);
        free(data);
    }
    chunk->loaded = 1;
}

void chunk_unload(Chunk *chunk) {
    if(chunk->loaded) {
        if(chunk->rootTag) {
            ntb_destroyTag(chunk->rootTag);
        }
        chunk->loaded = 0;
    }
}

void chunk_findBlock(Chunk *chunk, BlockType blockType) {
    int i;
    if(!chunk->loaded) {
        chunk_load(chunk);
    }
    
    printf("Search %s\n",  block_toString(blockType));

    for(i = 0; i < chunk->sectionCount; i++) {
        struct TagByteArrayPayload *blocksArray;
        Tag * section = chunk->sections[i];
        
        unsigned char y = ntb_getByteByName(section,"Y");
        
        blocksArray = ntb_getByteArrayByName(section, "Blocks");
        int length = blocksArray->length;
        unsigned char *blocks = blocksArray->payload;
        
        for(int j = 0; j < length; j++) {
            if(blocks[j] == blockType) {
                
                
                
            
                printf("%s found at %d, %d , %d, %d\n",  block_toString(blocks[j]), chunk->x, chunk->z, y,j);
            } else {
                //printf("%s found at %d\n",  block_toString(blocks[j]), j);
            }
        }
    }
    
    /*
    for(i = 0; i < CHUNK_SIZE; i++) {
        if(chunk->blocks[i] == blockType) {
            printf("%s found at %d\n",  block_toString(blockType), i);
        }
    }*/
    
}

Space chunk_getSpace(Chunk * chunk) {
    Space space;
    space.minX = chunk->x;
    space.minY =  0;
    space.minZ = chunk->z;
    space.maxX = chunk->x+CHUNK_WIDTH;
    space.maxY = CHUNK_HEIGHT;
    space.maxZ = chunk->z+CHUNK_WIDTH;
    return space;
}


char *block_toString(BlockType blockType) {
    switch(blockType) {
        case BLOCK_AIR:
            return "Air";
        case BLOCK_STONE:
            return "Stone";
        case BLOCK_GRASS:
            return "Grass";     
        case BLOCK_GOLD_ORE:
            return "Gold Ore";     
        case BLOCK_IRON_ORE:
            return "Iron Ore"; 
        case BLOCK_COAL_ORE:
            return "Coal Ore"; 
        case BLOCK_DIAMOND_ORE:
            return "Diamond Ore";
        default:
            return "Unknown block";
    }
}
