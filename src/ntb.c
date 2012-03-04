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
    
    if(tag->name) {
        printf("Create named tag %d: %s\n", tag->type, tag->name);
    } else {
        printf("Create unnamed tag %d\n", tag->type);
    }
    
    switch(tag->type) {
        case TAG_END:
            printf("Create end tag\n");
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
        case TAG_STRING: {
                tag->payload = parseStringPayload(data+index, size-index, &indexUse);
                index += indexUse;
                printf("Create string tag: %s \n", (char *)tag->payload);
            }
            break;
        case TAG_LIST: {
                int i;
                
                int * length;
                
                struct TagListPayload *payload = smalloc(sizeof(struct TagListPayload));

                payload->type = *(data+index);
                length = (int *) (data+index+1);
                i = *length;
                payload->length = endian_swap(i);
                
                printf("Create list tag: type %d, size %d\n", payload->type, payload->length);
                index += 5;
                
                
                switch(payload->type) {
                    case TAG_DOUBLE: {
                            double *doubleBuffer;
                            unsigned long long *value;
                             unsigned long long uValue;
                            double *dValue;
                            char * bBuffer;
                            doubleBuffer = smalloc(sizeof(double) * payload->length);        
                            payload->payload = doubleBuffer;
                            
                            for(i = 0; i < payload->length ; i++) {
                                bBuffer = (char *) data+index+i*sizeof(double);
                                value = (unsigned long long *) bBuffer;

                                uValue = endian_swap_8(*value);
                                dValue =(double*) &uValue;
                                doubleBuffer[i] = (*dValue);
                                
                                printf("Add double %f\n", doubleBuffer[i]);
                            }
                            index += sizeof(double) *payload->length;
                            
                        }                
                    case TAG_COMPOUND: {
                            Tag **tagList;
                            tagList = smalloc(sizeof(Tag *) * payload->length);
                            payload->payload = tagList ;

                            for(i = 0; i < payload->length ; i++) {
                                tagList[i] = ntb_parseData(data+index, size-index, &indexUse);
                                index += indexUse;
                            }
                        }
                    break;
                    default:
                        printf("Unsupported list type %d\n", payload->type);
                
                }
                
                tag->payload = payload;
                
            }
            break;
        case TAG_COMPOUND: {
                Tag *payload;
                struct TagCompoundPayload *lastPayload = smalloc(sizeof(struct TagCompoundPayload));
                printf("Create compound tag\n");
                tag->payload = lastPayload;
                do {
	                lastPayload->nextTagPayload = smalloc(sizeof(struct TagCompoundPayload));
	                payload = ntb_parseData(data+index, size-index, &indexUse);
	                lastPayload->payload = payload;
	                index +=indexUse;
                    lastPayload = lastPayload->nextTagPayload;
                    
                } while(payload->type != TAG_END);
                free(lastPayload);
                printf("Compound tag created\n");
            }
            break;
        default:
            if(tag->name) {
                /*printf("Unknown tag %d named: %s", tag->type, tag->name);*/
                printf("Unknown named tag %d , name size %d\n", tag->type, (int) strlen(tag->name));
            } else {
                printf("Unknown unnamed tag %d\n", tag->type);
            }
    
    }
    
    (*inIndexUse) = index;
    
    return tag;

}


static char *parseStringPayload(unsigned char * data, int size, int *indexUse) {
    int length;
    char *string;
    (void) size;
    length = data[1] + (data[0] << 8);
    string = smalloc(sizeof(char) * (length +1));
    memcpy(string, data+2, length);
    string[length] = '\0';
    *indexUse = 2+length;
    return string;
}

void ntb_destroyTag(Tag *tag) {
    switch(tag->type) {
        case TAG_END:
            break;
        case TAG_BYTE:
        case TAG_SHORT:
            break;
        case TAG_LIST: {
                int i;
                Tag **tagList;
                struct TagListPayload *listPayload = (struct TagListPayload *) tag->payload;
                tagList = listPayload->payload;
                for(i = 0; i < 0 ; i++) {
                    ntb_destroyTag(tagList[i]);
                }
                free(listPayload->payload);
        }
        case TAG_COMPOUND: {
                struct TagCompoundPayload *compoundPayload = ((struct TagCompoundPayload *) tag->payload)->nextTagPayload;
                TagType type = ((struct TagCompoundPayload *) tag->payload)->payload->type;
                printf("Destroying compound tag\n");
                ntb_destroyTag(((struct TagCompoundPayload *) tag->payload)->payload);

                while(type != TAG_END) {
                   struct TagCompoundPayload *next;
                   type = compoundPayload->payload->type;
                   next = compoundPayload->nextTagPayload;
                   ntb_destroyTag(compoundPayload->payload);
                   free(compoundPayload);
                   compoundPayload = next;
                }
                printf("Compound tag destroyed\n");
            }
            break;
        default:
            printf("Unknown tag %d to destroy\n", tag->type);
    }
    free(tag->payload);
    free(tag->name);
    free(tag);
}


