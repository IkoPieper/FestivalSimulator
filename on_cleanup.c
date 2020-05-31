#include "on_cleanup.h"
 
void on_cleanup(object_t* obj, video_t* vid, sound_t* snd, 
    verletbox_t* vbox, bool* keys) {
	
    // objects:
	object_clean_up(obj);
	
    // video:
    SDL_GL_DeleteContext(vid->glcontext);
    SDL_DestroyRenderer(vid->renderer);
    SDL_FreeSurface(vid->surface);
    TTF_CloseFont(vid->font);
    free(vid);
    SDL_VideoQuit();
    
    // sound:
    while(Mix_Init(0)) {
        Mix_Quit();
    }
    Mix_CloseAudio();
    
    if (snd->num_songs > 0) {
        
        Mix_FreeMusic(snd->music);
        
        for (uint32_t n = 0; n < snd->num_songs; n++) {
            free(snd->songs[n]);
        }
        free(snd->songs);
    }
    free(snd);
    
    // fonts:
	TTF_Quit();
    
    // SDL:
	SDL_Quit();
    
    // verlet boxes:
	verletbox_free(vbox);
	
    // keys:
	free(keys);
	
}
