#include <pebble.h>

/**
 * Replace delim with '\0' and return an array pointing to each substring
 * Note, assumes all parameters are correct
 */
char **splittoarray(char *text, int length, char delim, int *amount) {

	// We will always have at least one substring
	*amount = 1;

	// Convert delim to \0 and count amount
	int i;
	for (i=0;i<length;i++) {
		if (*(text+i) == delim) {
			*amount += 1;
			*(text+i) = '\0';
		}
	}

	// Allocate our array
	char **strings = malloc(sizeof(char *) * (*amount));


	// Fill up strings
	int index = 0;
	char *part = text;
	for (i=0;i<=length;i++) {
		if ((i == length || *(text+i) == '\0' ) && index < *amount) {
			strings[index++] = part;
			part = text+i+1;
		}
	}

	return strings;
}

/**
 * Stack Overflow atoi, because I am not sure if pebble support atoi or not...
 * http://stackoverflow.com/questions/7957557/understanding-an-atoi-function/7957656#7957656
 */
int satoi(char* str)
{
    if(!str)
        return 0;

    int number = 0;
    char* p = str;

    while((*p >= '0') && (*p <= '9'))
    {
        number = number * 10 + (*p - '0');
        p++;
    } 
    return number;
}