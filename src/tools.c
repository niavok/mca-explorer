#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "tools.h"

void *smalloc(int size) {
    void *out = malloc(size);
    if(!out) {
        printf("Out of memory");
        exit(1);
    }
    return out;
}

char *string_cat(char *str1, char *str2) {
    char *out = smalloc(sizeof(char) * (strlen(str1) + strlen(str2) +1) );
    out[0] = '\0';
    strcat(out, str1);
    strcat(out, str2);
    
    return out;
}

char *string_clone(char *str) {
    char *out = smalloc(sizeof(char) * (strlen(str) + 1) );
    out[0] = '\0';
    strcat(out, str);
    
    return out;
}

char file_exists(char *path) {
    struct stat st;
    return stat(path,&st) == 0;
}
