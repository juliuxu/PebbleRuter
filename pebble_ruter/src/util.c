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

		if (*(text+i) == '\0' && index < *amount) {
			strings[index++] = part;
			part = text+i+1;
		}
	}

	return strings;
}