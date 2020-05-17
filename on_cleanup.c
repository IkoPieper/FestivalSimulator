#include "on_cleanup.h"
 
void on_cleanup(object_t* obj, video_t* vid, verletbox_t* vbox, bool* keys) {
	
	object_clean_up(obj);
	
    SDL_GL_DeleteContext(vid->glcontext);
    SDL_DestroyRenderer(vid->renderer);
    SDL_FreeSurface(vid->surface);
    free(vid);
    SDL_VideoQuit();
	TTF_Quit();
	SDL_Quit();
    
	verletbox_free(vbox);
	
	free(keys);
	
}
