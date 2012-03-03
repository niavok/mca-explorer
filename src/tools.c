#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

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

char **file_listDir(char *path, char fileOnly, int *count) {
        DIR *dir;
        int i;
        struct dirent *ent;
        char **out ;
        
        out = NULL;
        
        *count = 0;
        
        dir = opendir (path);
        if (dir != NULL) {

          /* Count files and directories*/
          while ((ent = readdir (dir)) != NULL) {
            if(ent->d_type == 8) {
                *count +=1;
            }            
          }
          
          rewinddir(dir);
          out = smalloc(sizeof(char*) * *count);
          i = 0;

          /* store files and directories */
          while ((ent = readdir (dir)) != NULL) {
            if(ent->d_type == 8) {
                out[i] = string_clone(ent->d_name);
                i += 1;
            }
          }
          closedir (dir);
        } else {
          /* could not open directory */
          perror ("");

        }
        return out;
    }

