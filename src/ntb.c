#include <stdlib.h>
#include <stdio.h>

#include <string.h>

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
            break;
        case TAG_BYTE: {
                char *payload = smalloc(sizeof(char));
                tag->payload = payload;
                (*payload) = data[index];
                index +=1;
	        }
            break;
        case TAG_SHORT: {
	            short *payload = smalloc(sizeof(short));
                tag->payload = payload;
                (*payload) = data[index+1] + (data[index] << 8);
                index +=2;
                }
            break;
        case TAG_COMPOUND: {
                Tag *payload;
                struct TagCompoundPayload *lastPayload = smalloc(sizeof(struct TagCompoundPayload));
                tag->payload = lastPayload;
                
                do {
	                lastPayload->nextTagPayload = smalloc(sizeof(struct TagCompoundPayload));
	                payload = ntb_parseData(data+index, size-index, &indexUse);
	                lastPayload->payload = payload;
	                index +=indexUse;
                    lastPayload = lastPayload->nextTagPayload;
                } while(payload->type != TAG_END);
                free(lastPayload);
            }
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
    int length;
    char *string;
    
    length = data[1] + (data[0] << 8);
    string = smalloc(sizeof(char) * (length +1));
    memcpy(string, data, length);
    string[length] = '\0';
    *indexUse = 2+length;
    return string;
}
