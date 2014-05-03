#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

typedef struct DictEntry {
	uint8_t key;
	
	enum {
		CSTRING,
		UINT8
	} type;

	union {
		char *cstring;
		uint8_t uint8;
	} value;

} dict_entry_t;

void send_message(dict_entry_t **dicts, uint8_t dicts_length, void (*success_callback)(void), void (*failure_callback)(void));
void destroy_messages(void);

void message_handler_outbox_sent_handler(DictionaryIterator *iterator, void *context);
void message_handler_outbox_failed_handler(DictionaryIterator *iterator, AppMessageResult reason, void *context);

#endif