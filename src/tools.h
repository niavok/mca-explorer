#ifndef TOOLS_H
#define TOOLS_H

void *smalloc(int size);

char *string_cat(char *str1, char *str2);

char *string_clone(char *str);

char file_exists(char *path);

#endif