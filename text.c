#include "text.h"

text_t* text_init() {
	
	text_t* txt = NULL;
	
	txt = (text_t*) malloc(sizeof(text_t));
	
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

SDL_Surface* text_print_to_surface(text_t* txt, char* str) {
	
	SDL_Surface* surf;
	SDL_Color fg_color = {255, 255, 255};
 	
 	surf = TTF_RenderText_Solid(txt->font, str, fg_color);
 	
	return(surf);
	
}
