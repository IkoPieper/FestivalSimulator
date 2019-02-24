#include "on_cleanup.h"
 
void on_cleanup(object_t* obj, verletbox_t* vbox, bool* keys) {
	
	//SDL_SetVideoMode(400, 300, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	
	object_clean_up(obj);
	
	verletbox_free(vbox);
	
	free(keys);
	
	SDL_VideoQuit();
	
	
	TTF_Quit();
	SDL_Quit();
}
