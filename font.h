#ifndef _FONT_H_
    #define _FONT_H_
 
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
 
SDL_Surface* font_draw_text(TTF_Font* FontToDraw, const char* Text);

#endif
