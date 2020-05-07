#ifndef _TEXT_H_
    #define _TEXT_H_

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

typedef struct text text_t;

struct text {
	char* str;
	uint32_t length;
	TTF_Font* font;
    uint32_t duration;		// how many frames the text is visible
};

text_t* text_init();
void text_add_string(text_t* txt, char* str);
void text_free(text_t* txt);
SDL_Surface* text_print_to_surface(text_t* txt, char* str);

#define TEXT_DURATION_DEFAULT 200

#endif
