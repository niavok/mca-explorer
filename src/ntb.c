#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
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
        case TAG_INT: {
                unsigned char *bBuffer;
	            int *payload = smalloc(sizeof(int));
                uint32_t u;
                bBuffer = data+index;
                
                tag->payload = payload;
                u = endian_swap((* (int *)bBuffer));
                (*payload) = *((int *)(&u));
                index +=4;
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
                }
            break;
        case TAG_LONG: {
                unsigned char *bBuffer;
	            long long *payload = smalloc(sizeof(long long));
                unsigned long long u;
                bBuffer = data+index;
                
                tag->payload = payload;
                u = endian_swap_8((*bBuffer));
                (*payload) = *((long long *)(&u));
                index +=8;
                }
            break; 
        case TAG_BYTE_ARRAY: {
                int i;
                int * length;
                struct TagByteArrayPayload *payload = smalloc(sizeof(struct TagByteArrayPayload));
                length = (int *) (data+index);
                i = *length;
                payload->length = endian_swap(i);
                index += 4;
                
                
                payload->payload = smalloc(sizeof(unsigned char) * payload->length);
                memcpy(payload->payload, data+index, payload->length);
                index += payload->length;
                tag->payload = payload;
            }
            break;
        case TAG_STRING: {
                tag->payload = parseStringPayload(data+index, size-index, &indexUse);
                index += indexUse;
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
                
                index += 5;
                
                
                switch(payload->type) {
                    case TAG_BYTE: {
                            char * bBuffer;
                            bBuffer = smalloc(sizeof(char) * payload->length);        
                            payload->payload = bBuffer;
                            memcpy(bBuffer, data+index,  payload->length);
                                
                            index += sizeof(char) *payload->length;
                            
                        }   
                    break;         
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
                                
                            }
                            index += sizeof(double) *payload->length;
                            
                        }      
                    break;          
                    case TAG_COMPOUND: {
                            Tag **tagList;
                            tagList = smalloc(sizeof(Tag *) * payload->length);
                            payload->payload = tagList ;
                            for(i = 0; i < payload->length ; i++) {
                                Tag *tag = smalloc(sizeof(Tag));
                                tag->type = payload->type;
                                tag->name = NULL;
                                tag->payload = parseCompountPayload(data+index,  size-index, &indexUse);
                                index += indexUse;
                                
                                tagList[i] = tag;
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
                tag->payload = parseCompountPayload(data+index,  size-index, &indexUse);
                index += indexUse;
            }
            break;
        case TAG_INT_ARRAY: {
                    int i;
                    int * length;
                    int * intBuffer;
                    struct TagIntArrayPayload *payload = smalloc(sizeof(struct TagIntArrayPayload));
                    length = (int *) (data+index);
                    i = *length;
                    payload->length = endian_swap(i);
                    index += 4;
                    
                    payload->payload = smalloc(sizeof(unsigned int) * payload->length);
                    
                    intBuffer = (int *) (data+index);
                    for(i = 0; i < payload->length; i++) {
                        payload->payload[i] = endian_swap(intBuffer[i]);
                    }
                    index += sizeof(unsigned int) * payload->length;
                    tag->payload = payload;
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
    struct TagCompoundPayload *firstPayload = lastPayload;
    
    do {
        lastPayload->nextTagPayload = smalloc(sizeof(struct TagCompoundPayload));
        payload = ntb_parseData(data+index, size-index, &indexUse);
        lastPayload->payload = payload;
        index +=indexUse;
        lastPayload = lastPayload->nextTagPayload;
        
    } while(payload->type != TAG_END);
    free(lastPayload);
    
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
        case TAG_LONG:
        case TAG_INT:
        case TAG_FLOAT:
        case TAG_STRING:
            break;
        case TAG_BYTE_ARRAY: {
                struct TagByteArrayPayload *byteArrayPayload = (struct TagByteArrayPayload *) tag->payload;
                free(byteArrayPayload->payload);
            }
        break;
        case TAG_LIST: {
                int i;
                struct TagListPayload *listPayload = (struct TagListPayload *) tag->payload;
                
                switch(listPayload->type) {
                    case TAG_BYTE:
                    case TAG_FLOAT:
                    case TAG_DOUBLE:
                        free(listPayload->payload);
                    break;
                    case TAG_COMPOUND: {
                        Tag **tagList;
                        tagList = listPayload->payload;
                        for(i = 0; i < listPayload->length ; i++) {
                            ntb_destroyTag(tagList[i]);
                        }

                        free(listPayload->payload);
                    }
                    break;
                    default:
                        printf("Unknown list type %d to destroy\n", listPayload->type);
                }
        }
        break;
        case TAG_COMPOUND: {
                struct TagCompoundPayload *compoundPayload = ((struct TagCompoundPayload *) tag->payload)->nextTagPayload;
                TagType type = ((struct TagCompoundPayload *) tag->payload)->payload->type;
                ntb_destroyTag(((struct TagCompoundPayload *) tag->payload)->payload);

                while(type != TAG_END) {
                   struct TagCompoundPayload *next;
                   type = compoundPayload->payload->type;
                   next = compoundPayload->nextTagPayload;
                   ntb_destroyTag(compoundPayload->payload);
                   free(compoundPayload);
                   compoundPayload = next;
                }
            }
            break;
        case TAG_INT_ARRAY: {
                struct TagIntArrayPayload *intArrayPayload = (struct TagIntArrayPayload *) tag->payload;
                free(intArrayPayload->payload);
            }
        break;
        default:
            printf("Unknown tag %d to destroy\n", tag->type);
    }
    free(tag->payload);
    free(tag->name);
    free(tag);
}

unsigned char ntb_getByteByName(Tag *parent, char* name) {
    struct TagCompoundPayload *childCompoundPayload;
    childCompoundPayload = parent->payload;
    do {
        if(strcmp(childCompoundPayload->payload->name, name) == 0) {
            return *((char *) (childCompoundPayload->payload->payload));
        }
        childCompoundPayload = childCompoundPayload->nextTagPayload;
    }while(childCompoundPayload->payload->type != TAG_END);
    return 0;
}

struct TagByteArrayPayload *ntb_getByteArrayByName(Tag *parent, char* name) {
    struct TagCompoundPayload *childCompoundPayload = parent->payload;
    do {
        if(strcmp(childCompoundPayload->payload->name, name) == 0) {
            return childCompoundPayload->payload->payload;
        }
        childCompoundPayload = childCompoundPayload->nextTagPayload;
    }while(childCompoundPayload->payload->type != TAG_END);
    return NULL;
}


struct Tag *ntb_getTagByName(Tag *parent, char* name) {
    struct TagCompoundPayload *childCompoundPayload = parent->payload;
    do {
        if(strcmp(childCompoundPayload->payload->name, name) == 0) {
            return childCompoundPayload->payload;
        }
        childCompoundPayload = childCompoundPayload->nextTagPayload;
    }while(childCompoundPayload->payload->type != TAG_END);
    return NULL;
}

char *ntb_typeToString(TagType type) {
    char *typeStr;
    switch(type) {
        case TAG_END:
            typeStr = "End";
            break;
        case TAG_BYTE:
            typeStr = "Byte";
        break;
        case TAG_SHORT:
            typeStr = "Short";
        break;
        case TAG_LONG:
            typeStr = "Long";
        break;
        case TAG_INT:
            typeStr = "Int";
        break;
        case TAG_FLOAT:
            typeStr = "Float";
        break;
        case TAG_STRING:
            typeStr = "String";
        break;
        case TAG_BYTE_ARRAY:
            typeStr = "Byte array";
        break;
        case TAG_LIST:
            typeStr = "List";
        break;
        case TAG_COMPOUND:
            typeStr = "Compound";
        break;
        case TAG_INT_ARRAY:
            typeStr = "Int array";
        break;
        default:
            typeStr = "Unknown";
    }
    return typeStr;
}

void ntb_print(Tag *tag, int indent) {
    char *typeStr;

    typeStr = ntb_typeToString(tag->type);

    if(tag->name == NULL) {
        printf("%*s+ Unamed %s tag",indent," ", typeStr);
    } else {
        printf("%*s+ %s tag '%s'",indent," ", typeStr, tag->name);
    }
    
    switch(tag->type) {
        case TAG_END:
            break;
        case TAG_BYTE:
            printf(" > %d", *((unsigned char *) tag->payload));
            printf("\n");
        break;
        case TAG_SHORT:
            printf(" > %d", *((unsigned short *) tag->payload));
            printf("\n");
        break;
        case TAG_LONG:
            printf(" > %lld", *((long long *) tag->payload));
            printf("\n");
        break;
        case TAG_INT:
            printf(" > %d", *((int *) tag->payload));
            printf("\n");
        break;
        case TAG_FLOAT:
            printf(" > %f", *((float *) tag->payload));
            printf("\n");
        break;
        case TAG_DOUBLE:
            printf(" > %f", *((double *) tag->payload));
            printf("\n");
        break;
        case TAG_STRING:
            printf(" > %s", (char*) tag->payload);
            printf("\n");
        break;
        case TAG_BYTE_ARRAY: {
            struct TagByteArrayPayload *arrayPayload = tag->payload;
            printf(" > %d bytes", arrayPayload->length);
            printf("\n");
        }
        break;
        case TAG_LIST: {
            struct TagListPayload *listPayload = tag->payload;
            printf(" > %d %s", listPayload->length, ntb_typeToString(listPayload->type));
            
           switch(listPayload->type) {
                case TAG_END:
                    break;
                case TAG_BYTE:
                break;
                case TAG_SHORT:
                break;
                case TAG_LONG:
                break;
                case TAG_INT:
                break;
                case TAG_FLOAT:
                break;
                case TAG_DOUBLE:
                break;
                case TAG_STRING:
                break;
                case TAG_BYTE_ARRAY:
                break;
                case TAG_LIST: {
                }
                break;
                case TAG_COMPOUND: {
                    Tag **tags = listPayload->payload;
                    printf("\n");
                    for(int i =0; i< listPayload->length; i++) {
                        ntb_print(tags[i], indent + 3);
                    }
                    
                }
                break;
                case TAG_INT_ARRAY:
                break;
            } 
            printf("\n");
        }
            
        break;
        case TAG_COMPOUND: {
            struct TagCompoundPayload *compoundPayload = tag->payload;
            printf("\n");
            
            while(compoundPayload->payload->type != TAG_END) {
                ntb_print(compoundPayload->payload, indent + 3);
                compoundPayload = compoundPayload->nextTagPayload;
            }
        }
        break;
        case TAG_INT_ARRAY: {
            struct TagIntArrayPayload *arrayPayload = tag->payload;
            printf(" > %d bytes", arrayPayload->length);
            printf("\n");
        }
        break;
    }
}





