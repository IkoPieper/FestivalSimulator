#include "text.h"

text_t* text_init() {
	
	text_t* txt = (text_t*) malloc(sizeof(text_t));
	
	txt->str = NULL;
	txt->length = 0;
    txt->duration = TEXT_DURATION_DEFAULT;
	
	if((txt->font = TTF_OpenFont("FreeSansBold.ttf", 16)) == NULL) {
		fprintf(stderr, "Error: Font FreeSansBold.ttf is missing!");
		return(NULL);
	}
	
	return(txt);
}

void text_add_string(text_t* txt, char* str) {
	
	uint32_t length;
	
	length = 0;
	while (str[length] != '\0') {
		length++;
	}

	txt->str = (char *) malloc((length + 1) * sizeof(char));
	
	length = 0;
	while (str[length] != '\0') {
		txt->str[length] = str[length];
		length++;
	}
	txt->str[length] = '\0';
	txt->length = length + 1;
	
}

void text_free(text_t* txt) {
	
	TTF_CloseFont(txt->font);
	free(txt->str);
	free(txt);
}

SDL_Surface* text_print_to_surface(
    TTF_Font* font, char* str, uint32_t length) {
	
    uint32_t width = 200;    // maximum text width [pixel]
	SDL_Color fg_color = {  0,   0,   0};   // text color
    SDL_Color bg_color = {252, 255, 190};   // text background color
 	
    SDL_Surface* surf = 
        TTF_RenderText_Shaded(font, str, fg_color, bg_color);
    
    if (surf->w < width) {
        return(surf);
    }
    
    // create a text surface with more than one row:
    SDL_FreeSurface(surf);
    
    uint32_t height_row = 20; // [pixel]
    
    uint32_t i = 0;     // leter
    uint32_t x = 0;     // x position on surface
    uint32_t y = 0;     // y position on surface
    
    // first, find the surface height we need:
	while (i < length) {
         
        // find word length:
        uint32_t word_length = 1;
        uint32_t offset = 0;
        while (str[i + offset] != ' ' && i + offset < length) {
            word_length++;
            offset++;
        }
        
        // copy word:
        char str_word[word_length + 1];
        for (uint32_t j = 0; j < word_length; j++) {
            str_word[j] = str[i];
            i++;
        }
        str_word[word_length] = '\0';
        
        // render word:
        SDL_Surface* surf_word = 
            TTF_RenderText_Shaded(font, str_word, fg_color, bg_color);
        
        // go to next row if word is over the edge:
        if (x + surf_word->w > width) {
            x = 0;
            y = y + height_row;
        }
        
        x = x + surf_word->w;
        
        SDL_FreeSurface(surf_word);
	}
 	
    uint32_t height = y + height_row;
    
    // create transparent surface:
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        surf = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height,
        32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    #else
        surf = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height,
        32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    #endif
    
    // print words on transparent surface:
    i = 0;
    x = 0;
    y = 0;
    
	while (i < length) {
         
        // find word length:
        uint32_t word_length = 1;
        uint32_t offset = 0;
        while (str[i + offset] != ' ' && i + offset < length) {
            word_length++;
            offset++;
        }
        
        // copy word:
        char str_word[word_length + 1];
        for (uint32_t j = 0; j < word_length; j++) {
            str_word[j] = str[i];
            i++;
        }
        str_word[word_length] = '\0';
        
        // render word:
        SDL_Surface* surf_word = 
            TTF_RenderText_Shaded(font, str_word, fg_color, bg_color);
        
        // go to next row if word is over the edge:
        if (x + surf_word->w > surf->w) {
            x = 0;
            y = y + height_row;
        }
        
        surface_on_draw(surf, surf_word, x, y);
        x = x + surf_word->w;
        
        SDL_FreeSurface(surf_word);
	}
    
	return(surf);
	
}
