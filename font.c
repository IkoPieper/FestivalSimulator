#include "font.h"

SDL_Surface* font_draw_text(TTF_Font* FontToDraw, const char* Text) {
 	SDL_Color tmpfontcolor = {255, 255, 255};
 	SDL_Color tmpfontbgcolor = {0, 0, 0};
	return(TTF_RenderText_Shaded(FontToDraw, Text, tmpfontcolor, tmpfontbgcolor));
} 
