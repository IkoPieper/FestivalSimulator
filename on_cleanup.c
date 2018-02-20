#include "on_cleanup.h"
 
void on_cleanup(object_t* obj) {
	
	SDL_SetVideoMode(400, 300, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	
	while (obj->prev_object != NULL) {
		if (obj->surface != NULL) {
			SDL_FreeSurface(obj->surface);
		}
		if (obj->font != NULL) {
			TTF_CloseFont(obj->font);
		}
		obj = object_remove(obj, obj->id);
	}
	
	TTF_Quit();
	SDL_Quit();
}
