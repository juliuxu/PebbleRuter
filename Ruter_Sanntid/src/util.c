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

// http://stackoverflow.com/questions/21150193/logging-enums-on-the-pebble-watch
char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}