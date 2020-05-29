#ifndef _TEXT_H_
    #define _TEXT_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include "surface.h"

typedef struct text text_t;

struct text {
	char* str;
	uint32_t length;
	TTF_Font* font;
    uint32_t duration;		// how long the text is visible
};

text_t* text_init();
void text_add_string(text_t* txt, char* str);
void text_free(text_t* txt);
SDL_Surface* text_print_to_surface(
    TTF_Font* font, char* str, uint32_t length);

#define TEXT_DURATION_DEFAULT 200

#endif
