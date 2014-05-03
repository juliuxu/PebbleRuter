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
#define MAX_MESSAGES_IN_QUEUE 100

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

/**
 * AppMessage Callbacks
 */
void message_handler_outbox_sent_callback(DictionaryIterator *iterator, void *context) {

	message_t *message = message_queue;

	// Set that we are done sending a message
	messages_sending = false;

	// Set the next head message
	message_queue = message->next;
	if (message_queue == NULL) {
		message_queue_tail = NULL;
	}

	// Call the message success_callback
	if (message->success_callback != NULL) {
		message->success_callback();
	}

	// Destroy the message
	destroy_message(message);

	// Continue onto the next message
	send_next_message();
}

void message_handler_outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	// Set that we are done sending a message
	messages_sending = false;

	// Continue onto the next message
	send_next_message();
}


/**
 * Send multiple dicts in a message.
 * Add the message to the message queue and try to send it
 */
void send_message(dict_entry_t **dicts, uint8_t dicts_length, void (*success_callback)(void), void (*failure_callback)(void)) {

	/**
	 * Check if there is room in the message queue
	 */
	if (!(num_messages < MAX_MESSAGES_IN_QUEUE)) {
		APP_LOG(APP_LOG_LEVEL_WARNING, "Message Queue Is Full!");

		// Call the failure_callback
		if (failure_callback != NULL) {
			failure_callback();
		}

		return;
	}

	/**
	 * Store the message
	 */
	message_t *message = malloc(sizeof(message_t));

	message->dicts = dicts;
	message->dicts_length = dicts_length;

	message->retries = RETRIES;

	message->success_callback = success_callback;
	message->failure_callback = failure_callback;

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
	uint8_t i;
	for(i=0;i<message->dicts_length;i++) {
		if (message->dicts[i]->type == CSTRING) {
			free(message->dicts[i]->value.cstring);
		}
		free(message->dicts[i]);
	}
	free(message);

	// Decrement the message queue counter
	num_messages--;
}

/**
 * Destroy all messages in queue
 */ 
void destroy_messages(void) {
	message_t *message;

	while(message_queue) {
		message = message_queue;
		message_queue = message_queue->next;
		destroy_message(message);
	}

}

/**
 * Try to send the next message in queue
 */
static void send_next_message(void) {

 	uint8_t i;

	// Grab the first message in queue
	message_t *message = message_queue;

	// See if we are already in progress of sending a message
	if (messages_sending) {
		return;
	}

	// See if the queue is empty
	if (message == NULL) {
		return;
	}

	// See if the current message has anymore retries
	if (message->retries <= 0) {

		// Call the failure_callback
		if (message->failure_callback != NULL) {
			message->failure_callback();
		}

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

 	// Add our dicts
	for(i=0;i<message->dicts_length;i++) {
		if (message->dicts[i]->type == CSTRING) {
 			dict_write_cstring(iter, message->dicts[i]->key, message->dicts[i]->value.cstring);		
		}
		else if (message->dicts[i]->type == UINT8) {
 			dict_write_uint8(iter, message->dicts[i]->key, message->dicts[i]->value.uint8);
		}
	}

	// Fire away!
 	app_message_outbox_send();

 	// Set that we are sending a message
 	messages_sending = true;

 	// Decrement the retries counter
 	message->retries--;

}