#ifndef UTIL_H
#define UTIL_H

char **splittoarray(char *text, int length, char delim, int *amount);
int satoi(char* str);
char *translate_error(AppMessageResult result);

#endif