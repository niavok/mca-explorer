#ifndef REGION_H
#define REGION_H

#include "world.h"

void region_init(Region *region, char *fileName, char *basePath);

void region_findBlock(Region *region, BlockType blockType, Space space);

void region_load(Region *region);

void region_unload(Region *region);

void region_destroy(Region *region);

#endif

