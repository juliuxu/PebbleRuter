/**
 * Message Handler
 *
 * Improvments over default pebble app message:
 *  - Persistent message, retry a few times if sending failes
 *  - Success and Error callback
 *
 * Based on: https://github.com/smallstoneapps/message-queue
 *
 * Author: Julian Jark
 */

#include <pebble.h>
#include "message-handler.h"

#define RETRIES 3
#define MAX_MESSAGES_IN_QUEUE 10

/**
 * Message Struct
 * FIFO queue
 */
typedef struct Message {

	dict_entry_t **dicts;
	uint8_t dicts_length;

	uint8_t retries;

	void (*success_callback)(void);
	void (*failure_callback)(void);

	struct Message *next;

} message_t;

/**
 * Global variables
 */
static message_t *message_queue;
static message_t *message_queue_tail; // For O(1) insertion
static uint8_t num_messages = 0;

static bool messages_sending = false;

/**
 * Functions
 */
static void destroy_message(message_t *message);
static void send_next_message(void);

// http://stackoverflow.com/questions/21150193/logging-enums-on-the-pebble-watch
static char *translate_error(AppMessageResult result) {
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

/**
 * Create and return a pointer to a dict_entry_t array
 */
dict_entry_t **dict_entries_create(uint8_t dicts_length) {

	uint8_t i;

	// Allocate the pointer to the dicts
	dict_entry_t **dicts = (dict_entry_t **) malloc(sizeof(dict_entry_t *) * dicts_length);

	// Allocate the dicts
	for(i=0;i<dicts_length;i++) {
		dicts[i] = (dict_entry_t *) malloc(sizeof(dict_entry_t));
	}

	return dicts;
}

/**
 * Strdup implementation
 */
char *strdup(const char *str) {
	char *copy;
	copy = (char *) malloc(strlen(str)+1);
	if (copy != NULL) {
		strcpy(copy, str);
	}
	return copy;
}

/**
 * AppMessage Callbacks
 */
void message_handler_outbox_sent_handler(DictionaryIterator *iterator, void *context) {
APP_LOG(APP_LOG_LEVEL_DEBUG, "message_handler_outbox_sent_handler");

	// Should the message queue be empty, just return
	if (message_queue == NULL) {
		return;
	}

	message_t *message = message_queue;

	// Set that we are done sending a message
	messages_sending = false;

	// Set the next head message
	message_queue = message->next;

	// Call the message success_callback
	if (message->success_callback != NULL) {
		message->success_callback();
	}

	// Destroy the message
	destroy_message(message);

	// Continue onto the next message
	send_next_message();
}

void message_handler_outbox_failed_handler(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
//APP_LOG(APP_LOG_LEVEL_DEBUG, "message_handler_outbox_failed_handler");
APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Outbox Failed!: %s", translate_error(reason));

	// Set that we are done sending a message
	messages_sending = false;

	// Try again or continue onto the next message
	send_next_message();
}


/**
 * Send multiple dicts in a message.
 * Add the message to the message queue and try to send it
 */
void send_message(dict_entry_t **dicts, uint8_t dicts_length, void (*success_callback)(void), void (*failure_callback)(void)) {
APP_LOG(APP_LOG_LEVEL_DEBUG, "send_message");

	/**
	 * Check if there is room in the message queue
	 */
	if (!(num_messages < MAX_MESSAGES_IN_QUEUE)) {
		APP_LOG(APP_LOG_LEVEL_WARNING, "Message Queue Is Full!");

		uint8_t i;
		for(i=0;i<dicts_length;i++) {
			if (dicts[i]->type == CSTRING) {
				free(dicts[i]->value.cstring);
			}
			free(dicts[i]);
		}
		free(dicts);

		// Call the failure_callback
		if (failure_callback != NULL) {
			failure_callback();
		}

		return;
	}

	/**
	 * Store the message
	 */
	message_t *message = (message_t *) malloc(sizeof(message_t));

	message->dicts = dicts;
	message->dicts_length = dicts_length;

	message->retries = RETRIES;

	message->success_callback = success_callback;
	message->failure_callback = failure_callback;

	message->next = NULL;

	// Increment the message_queue counter
	num_messages++;

	/**
	 * Add to the end of our queue
	 */
	if (message_queue == NULL) {
		message_queue = message;
		message_queue_tail = message;
	}
	else {
		// If message_queue != NULL we can also assume message_queue_tail != NULL
		message_queue_tail->next = message;
		message_queue_tail = message;
	}

	/**
	 * Send next message
	 */
	send_next_message();

}

/**
 * Destroy a message
 */
static void destroy_message(message_t *message) {
APP_LOG(APP_LOG_LEVEL_DEBUG, "destroy_message");

	// Free the dicts, and possible a cstring
	uint8_t i;
	for(i=0;i<message->dicts_length;i++) {
		if (message->dicts[i]->type == CSTRING) {
			free(message->dicts[i]->value.cstring);
		}
		free(message->dicts[i]);
	}

	// Free the pointer to the dicts
	free(message->dicts);

	// Free the message
	free(message);

	// Decrement the message queue counter
	num_messages--;
}

/**
 * Destroy all messages in queue
 */ 
void destroy_messages(void) {
// APP_LOG(APP_LOG_LEVEL_DEBUG, "destroy_messages");
	message_t *message;

	while(message_queue != NULL) {
		message = message_queue;
		message_queue = message_queue->next;
		destroy_message(message);
	}

}

/**
 * Try to send the next message in queue
 */
static void send_next_message(void) {
// APP_LOG(APP_LOG_LEVEL_DEBUG, "send_next_message");

	// Grab the first message in queue
	message_t *message = message_queue;

	// See if we are already in progress of sending a message
	if (messages_sending) {
		return;
	}

	// See if the queue is empty
	else if (message == NULL) {
		return;
	}

	// See if the current message has anymore retries
	else if (message->retries <= 0) {
		// Call the failure_callback
		if (message->failure_callback != NULL) {
			message->failure_callback();
		}

		// Set the next head message
		message_queue = message->next;

		// Destroy the message
		destroy_message(message);

		// Continue onto the next message
		send_next_message();
		return;
	}

	/**
	 * Send the message!	
	 */
	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending message to phone:");
 	// Add our dicts
 	uint8_t i;
	for(i=0;i<message->dicts_length;i++) {
		if (message->dicts[i]->type == CSTRING) {
 			dict_write_cstring(iter, message->dicts[i]->key, message->dicts[i]->value.cstring);
 			APP_LOG(APP_LOG_LEVEL_DEBUG, "key: %d cstring: %s", message->dicts[i]->key, message->dicts[i]->value.cstring);	
		}
		else if (message->dicts[i]->type == UINT8) {
 			dict_write_uint8(iter, message->dicts[i]->key, message->dicts[i]->value.uint8);
 			APP_LOG(APP_LOG_LEVEL_DEBUG, "key: %d uint8: %d", message->dicts[i]->key, message->dicts[i]->value.uint8);	 			
		}
	}

	// Fire away!
 	app_message_outbox_send();

 	// Set that we are sending a message
 	messages_sending = true;

 	// Decrement the retries counter
 	message->retries--;

}