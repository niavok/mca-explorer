#include <stdlib.h>
#include <stdio.h>

#include "tools.h"
#include "ntb.h"

static char *parseStringPayload(unsigned char * data, int size, int *indexUse);

Tag *ntb_parseData(unsigned char * data, int size, int *inIndexUse) {
    int index = 0;
    int indexUse;
    Tag *tag = smalloc(sizeof(Tag));
    
    tag->type = data[index];
    index +=1;
    tag->name = parseStringPayload(data+index, size -index,  &indexUse);
    index += indexUse;
    tag->payload = NULL;
    
    
    
    switch(tag->type) {
        case TAG_END:
            printf("Unexpected tag end");
            break;
        case TAG_BYTE:
            char *payload = smalloc(sizeof(char));
            tag->payload = payload;
            (*payload) = data[index];
            index +=1;
            break;
        case TAG_SHORT:
            tag->payload = smalloc(sizeof(shord));
            (*tag->payload) = data[index+1] + (data[index] << 8);
            index +=2;
            break;
        default:
            if(tag->name) {
                printf("Unknown tag %d named: %s", tag->type, tag->name);
            } else {
                printf("Unknown unnamed tag %d", tag->type);
            }
    
    }
    
    (*inIndexUse) = index;
    
    return tag;

}


static char *parseStringPayload(unsigned char * data, int size, int *indexUse) {
    char *string;
    
    *size = data[1] + (data[0] << 8);
    string = smalloc(sizeof(char) * (*size +1));
    memcpy(string, data, *size);
    string[*size] = '\0';
    return string;
}
