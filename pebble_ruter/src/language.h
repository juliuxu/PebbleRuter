#ifndef LANGAUGE_H
#define LANGAUGE_H

typedef enum {
	NO,
	EN
} language_t;

void set_language(language_t lang);
char *get_language_string(int i);

#endif