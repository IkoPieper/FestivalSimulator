#ifndef _TEXT_H_
    #define _TEXT_H_

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

typedef struct text text_t;

struct text {
	unsigned int id;
	text_t* prev;
	text_t* next;
	char* str;
	unsigned int length;
	TTF_Font* font;
};

text_t* text_init(unsigned int id);
void text_add_string(text_t* txt, char* str);
void text_free(text_t* text);
SDL_Surface* text_print_to_surface(text_t* txt, char* str);

#endif
