#ifndef REGION_H
#define REGION_H

#include "world.h"

void region_init(Region *region, char *fileName, char *basePath);

void region_destroy(Region *region);

#endif

