#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include "tools.h"
#include "ntb.h"

static char *parseStringPayload(unsigned char * data, int size, int *indexUse);
static struct TagCompoundPayload *parseCompountPayload(unsigned char * data, int size, int *indexUse);

Tag *ntb_parseData(unsigned char * data, int size, int *inIndexUse) {
    int index = 0;
    int indexUse;
    Tag *tag = smalloc(sizeof(Tag));
    
    
    
    tag->type = data[index];
    index +=1;
    if(tag->type != TAG_END) {
        tag->name = parseStringPayload(data+index, size -index,  &indexUse);
        index += indexUse;
    } else {
        tag->name = NULL;
    }
    tag->payload = NULL;
    
    if(tag->name) {
        printf("Create named tag %d: %s\n", tag->type, tag->name);
    } else {
        printf("Create unnamed tag %d\n", tag->type);
    }
    
    switch(tag->type) {
        case TAG_END:
            printf("End tag\n");
            break;
        case TAG_BYTE: {
                char *payload = smalloc(sizeof(char));
                tag->payload = payload;
                (*payload) = data[index];
                index +=1;
                 printf("Create byte : %d \n", *payload);
	        }
            break;
        case TAG_SHORT: {
	            short *payload = smalloc(sizeof(short));
                tag->payload = payload;
                (*payload) = data[index+1] + (data[index] << 8);
                index +=2;
                printf("Create short : %d \n", *payload);
                }
            break;
        case TAG_INT: {
                unsigned char *bBuffer;
	            int *payload = smalloc(sizeof(int));
                unsigned u;
                bBuffer = data+index;
                
                tag->payload = payload;
                u = endian_swap((*bBuffer));
                (*payload) = *((int *)(&u));
                index +=4;
                printf("Create int : %d \n", *payload);
                }
            break;
        case TAG_FLOAT: {
                unsigned char *bBuffer;
	            float *payload = smalloc(sizeof(float));
	            unsigned u;
	            bBuffer = data+index;
                tag->payload = payload;
                u = endian_swap((*bBuffer));
                (*payload) = *((float *)(&u));
                index +=4;
                printf("Create float : %f \n", *payload);
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
                    case TAG_FLOAT: {
                            float *floatBuffer;
                            unsigned *value;
                            unsigned uValue;
                            float *dValue;
                            char * bBuffer;
                            floatBuffer = smalloc(sizeof(float) * payload->length);        
                            payload->payload = floatBuffer;
                            
                            for(i = 0; i < payload->length ; i++) {
                                bBuffer = (char *) data+index+i*sizeof(float);
                                value = (unsigned*) bBuffer;

                                uValue = endian_swap(*value);
                                dValue =(float*) &uValue;
                                floatBuffer[i] = (*dValue);
                                
                                printf("Add float %f\n", floatBuffer[i]);
                            }
                            index += sizeof(float) *payload->length;
                            
                        }   
                    break;             
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
                    break;          
                    case TAG_COMPOUND: {
                            Tag **tagList;
                            tagList = smalloc(sizeof(Tag *) * payload->length);
                            payload->payload = tagList ;
                            printf("Begin Compound tag list %d\n", payload->length);
                            for(i = 0; i < payload->length ; i++) {
                                Tag *tag = smalloc(sizeof(Tag));
                                tag->type = payload->type;
                                tag->name = NULL;
                                tag->payload = parseCompountPayload(data+index,  size-index, &indexUse);
                                index += indexUse;
                                
                                tagList[i] = tag;
                            }
                            printf("End Compound tag list\n");
                        }
                    break;
                    default:
                        printf("Unsupported list type %d\n", payload->type);
                
                }
                
                tag->payload = payload;
                
            }
            break;
        case TAG_COMPOUND: {
                tag->payload = parseCompountPayload(data+index,  size-index, &indexUse);
                index += indexUse;
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

static struct TagCompoundPayload *parseCompountPayload(unsigned char * data, int size, int *inIndexUse) {
    Tag *payload;
    int index = 0;
    int indexUse;
    struct TagCompoundPayload *lastPayload = smalloc(sizeof(struct TagCompoundPayload));
    printf("Create compound tag\n");
    struct TagCompoundPayload *firstPayload = lastPayload;
    
    do {
        lastPayload->nextTagPayload = smalloc(sizeof(struct TagCompoundPayload));
        payload = ntb_parseData(data+index, size-index, &indexUse);
        lastPayload->payload = payload;
        index +=indexUse;
        lastPayload = lastPayload->nextTagPayload;
        
    } while(payload->type != TAG_END);
    free(lastPayload);
    printf("Compound tag created\n");
    
    (*inIndexUse) = index;
    
    return firstPayload;
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


