#include "festival.h"

bool toggle_fullscreen(bool fullscreen, video_t* vid) {
    
    uint32_t w = vid->surface->w;
    uint32_t h = vid->surface->h;
    
    if (fullscreen) {
                    
        SDL_SetWindowFullscreen(vid->window, 0);
        
        glViewport(0, 0, w, h);
        
        SDL_ShowCursor(SDL_ENABLE);
        
        return(false);
                    
    } else {
    
        SDL_SetWindowFullscreen(vid->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        
        int32_t w_window, h_window;
        SDL_GetWindowSize(vid->window, &w_window, &h_window);
        float scale =  (float) h_window / (float) h;
        float w_viewport = scale * (float) w;
        float border = ((float) w_window - w_viewport) / 2.0;
        glViewport(border, 0, w_viewport, (float) h_window);
        
        SDL_ShowCursor(SDL_DISABLE);
        
        return(true);
    }
}

bool on_execute() {
	
    bool running = true;				// program running?
    const bool VSYNC = true;            // enable vertical sync
	const uint16_t FPS = 60;			// frames per second
    float dt = 1.0;                     // time step
    uint32_t time_start, time_end;		// to measure time per frame
    //Uint32 time;						// to measure time for debug
    uint64_t frame = 0;					// current frame
    bool fullscreen = false;            // fullscreen mode
    uint8_t lock_fullscreen_key = 0;
	
    sound_t* snd = on_init_sound();
    
	video_t* vid = on_init_video(VSYNC);
	if (vid == NULL) {
		fprintf(stderr, "Initialization of sdl, ttf, or video failed!\n");
		return(true);
	}
    
    if (VSYNC) {
        dt = 60.0 / vid->fps;
        printf("Vertical Syncronisation enabled. fps = %d\n", vid->fps);
    } else {
        dt = 60.0 / (float) FPS;
    }
    
	object_t* obj = on_init_objects(vid, dt);
	if (obj == NULL) {
		fprintf(stderr, "Initialization of objects failed!\n");
		return(true);
	}
	
 	on_start(obj);  // does nothing
 	
 	verletbox_t* vbox = verletbox_init(obj);
 	
 	bool* keys = (bool*) malloc(9 * sizeof(bool));
	for (uint8_t i = 0; i < 9; i++) {
		keys[i] = false;
	}
    
	SDL_Event event;
	
	while (running) {
		
        if (!VSYNC) {
            time_start = SDL_GetTicks();
        }
		
        if (lock_fullscreen_key) {
            lock_fullscreen_key--;
        }
        
		while (SDL_PollEvent(&event)) {
			
			on_event(&event, keys);
            
            if (!lock_fullscreen_key && keys[KEY_FULLSCREEN]) {
                lock_fullscreen_key = 30;
                fullscreen = toggle_fullscreen(fullscreen, vid);
            }
            
			if (keys[KEY_ESCAPE] || event.type == SDL_QUIT) {
				running = 0;
			}
		}

		//time = SDL_GetTicks();
        printf("FRAME: %lu\n", frame);
		on_loop(obj, snd, vbox, keys, frame, dt);
		//printf("time for on_loop: %d\n", SDL_GetTicks() - time);
		//time = SDL_GetTicks();
		on_render(obj, vid, dt);
		//printf("time for on_render: %d\n", SDL_GetTicks() - time);
		
		// ensure constant frame rate:
        if (!VSYNC) {
            time_end = SDL_GetTicks();
            if(1000 / FPS > time_end - time_start) {
                SDL_Delay(1000 / FPS - (time_end - time_start));
            }
        }
        
		frame++;
	}
	
	// ALWAYS TODO: keep this up to date
	on_cleanup(obj, vid, snd, vbox, keys);
 
	return(false);
}
 
int main(int argc, char* argv[]) {
	
	on_execute();
	
	return(0);
}
