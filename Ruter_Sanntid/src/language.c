#include "language.h"

static const char *language_no[] = {

	// win-main.c
	"T-Bane",
	"Trikk",
	"Buss",
	"Tog",
	"Båt",

	// win-stops.c
	"Henter Lokasjon",
	// stops.c
	"Henter Nærmeste Stoppesteder",
	"Kunne Ikke Hente Lokasjon",
	"Ingen Nærme Stoppesteder Funnet",
	"Kunne Ikke Hente Stoppeste]der",

	// win-departures.c
	"Oppdatert: %s",
	"Henter Avganger",
	// departures.c
	"Ingen Avganger For Øyeblikket",
	"Kunne Ikke Hente Avganger",

	// AppMessage Failure text
	"Kunne ikke snakke med mobilen"

	// Command Timeout text
	"Fikk ikke svar fra mobilen",

};

static const char *language_en[] = {

	// win-main.c
	"Metro",
	"Tram",
	"Bus",
	"Train",
	"Ferry",

	// win-stops.c
	"Getting Location",
	// stops.c
	"Getting Closest Stops",
	"Could Not Get Location",
	"No Close Stops Found",
	"Could Not Get Stops",

	// win-departures.c
	"Updated: %s",
	"Getting Departures",
	// departures.c
	"No Departures At The moment",
	"Could Not Get Departures",

	// AppMessage Failure text
	"Error Sending Message To Phone"

	// Command Timeout text
	"Did not receive an answer from the phone",

};

static language_t language = NO;

void set_language(language_t lang) {
	language = lang;
}

char *get_language_string(int i) {

	const char **language_array;

	// Select the language
	if (language == NO) {
		language_array = language_no;
	}
	else {
		language_array = language_en;
	}

	return (char *) language_array[i];

}