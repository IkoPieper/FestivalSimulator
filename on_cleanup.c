#include "on_cleanup.h"
 
void on_cleanup(groups_t* grp, video_t* vid, sound_t* snd, 
    verletbox_t* vbox, bool* keys) {
	
    on_cleanup_area(grp, snd, vbox);
    
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
    sound_free(snd);
    
    // fonts:
	TTF_Quit();
    
    // SDL:
	SDL_Quit();
	
    // keys:
	free(keys);
	
}

void on_cleanup_area(groups_t* grp, sound_t* snd, verletbox_t* vbox) {
	
    // objects:
    object_t* obj = grp->obj_first;
    while (obj != NULL) {
        
        collisions_free(obj);
        items_free(obj);
        items_free_object_item_props(obj);
        
        obj = obj->next_object;
    }
    tasks_free(grp);
    
	object_clean_up(grp->obj_first);
    
    // groups:
    groups_free(grp);
    
    // verlet boxes:
	verletbox_free(vbox);
}
