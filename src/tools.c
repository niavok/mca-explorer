#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <zlib.h>

#include "tools.h"

void *smalloc(int size) {
    void *out = malloc(size);
    if(!out) {
        printf("Out of memory\n");
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

char **string_split(char *str, char split,  int *count) {
    unsigned i;
    char ** out;
    unsigned partIndex;
    unsigned length;
    
    *count = 1;
    
    for(i = 0; i < strlen(str); i++) {
        if(str[i] == split) {
            *count += 1;
        }
    }
    
    out = smalloc(sizeof(char*) * *count);
    
    partIndex = 0;
    length = 0;
    for(i = 0; i < strlen(str); i++) {
        if(str[i] == split) {
            out[partIndex] = string_subnstr(str, i-length, length);

            partIndex +=1;
            length = 0;
        } else {
            length +=1;
        }
    }
    out[partIndex] = string_subnstr(str, i-length, length);
    
    
    return out;
}

char *string_substr(char *string, int from) {
    char *out;
    out = smalloc(sizeof(char) * (strlen(string) - from +1));
    strncpy(out, string+from , strlen(string) - from);
    out[strlen(string) - from] = '\0';
    return out;
}

char *string_subnstr(char *string, int from, int length) {
    char *out;
    out = smalloc(sizeof(char) * (length + 1));
    strncpy(out, string+from , length);
    out[length] = '\0';
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
            if(ent->d_type == 8 || !fileOnly) {
                *count +=1;
            }            
          }
          
          rewinddir(dir);
          out = smalloc(sizeof(char*) * *count);
          i = 0;

          /* store files and directories */
          while ((ent = readdir (dir)) != NULL) {
            if(ent->d_type == 8 || !fileOnly) {
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
    
void array_free(char **array, int size) {
    int i;
     for(i = 0; i < size; i++) {
        free(array[i]);
    }
    free(array);
}

unsigned endian_swap(unsigned x) {
    return (x>>24) | 
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
}

unsigned long long endian_swap_8(unsigned long long ull) {
    return (ull >> 56) |
          ((ull<<40) & 0x00FF000000000000) |
          ((ull<<24) & 0x0000FF0000000000) |
          ((ull<<8) & 0x000000FF00000000) |
          ((ull>>8) & 0x00000000FF000000) |
          ((ull>>24) & 0x0000000000FF0000) |
          ((ull>>40) & 0x000000000000FF00) |
          (ull << 56);
}


unsigned char *zlib_inflate(unsigned char *inputbuffer,  int size, int *outputSize) {
    z_stream strm;
    int outSize  = 0;
    int bufferSize  = 0;
    int bufferSizeBlock  = 5000;
    unsigned char *out = NULL;
    int ret;

    *outputSize = 0;
    
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    
    ret = inflateInit(&strm);
    if (ret != Z_OK) {
        return NULL;
    }
    
    strm.avail_in = size;
    strm.next_in = inputbuffer;
    
    do {
        unsigned char * newBuffer;
        bufferSize += bufferSizeBlock;
        newBuffer = smalloc(sizeof(char) * bufferSize);
        memcpy(newBuffer, out, outSize);
        free(out);
        out = newBuffer;
    
        strm.avail_out = bufferSizeBlock;
        strm.next_out = out + outSize;
        ret = inflate(&strm, Z_NO_FLUSH);

            switch (ret) {
            case Z_STREAM_ERROR:
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&strm);
                return NULL;
            }
        outSize += bufferSizeBlock - strm.avail_out;
    } while(strm.avail_out == 0);
    
    (void)inflateEnd(&strm);
    
    *outputSize = outSize;
    return out;
}

