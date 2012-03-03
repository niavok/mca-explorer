#ifndef TOOLS_H
#define TOOLS_H

void *smalloc(int size);

char *string_cat(char *str1, char *str2);

char *string_clone(char *str);

char **string_split(char *str, char split, int *count);

char *string_subnstr(char *string, int from, int lenght);

char *string_substr(char *string, int from);

char file_exists(char *path);

char **file_listDir(char *path, char fileOnly, int *count);

void array_free(char **array, int size);

#endif

