#include <stdlib.h>
#include <stdio.h>
#include "tools.h"
#include "region.h"

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
    
    printf("X: '%s' ; Y: '%s'\n", nameSplit[1], nameSplit[2]);
    
    array_free(nameSplit, nameSplitCount);

    
}



void region_destroy(Region *region) {
    free(region->path);
}

