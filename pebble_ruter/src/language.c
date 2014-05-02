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
	"Kunne ikke hente lokasjon",
	"Ingen nærme stoppesder funnet",
	"Kunne ikke hente stoppesder",

	// win-departures.c
	"Oppdatert: %s",
	"Henter avganger",
	// departures.c
	"Ingen avganger for øyeblikket",
	"Kunne ikke hente avganger"

};

static const char *language_en[] = {

	// win-main.c
	"Metro",
	"Tram",
	"Bus",
	"Train",
	"Boat", // Ferry?

	// win-stops.c
	"Getting Location",
	// stops.c
	"Getting Closest Stops",
	"Could not get location",
	"No close stops found",
	"Could not get stops",

	// win-departures.c
	"Updated: %s",
	"Getting departures",
	// departures.c
	"No departures at the moment",
	"Could not get departures"

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