#include "festival.h"

bool change_area(groups_t* grp, uint8_t* area) {
    
    object_t* hero = grp->obj_hero;
    
    switch (*area) {
        
        case 1:
            if (hero->pos_x < 700.0 && hero->pos_y < 500.0) {
                *area = 2;
                return(true);
            }
            break;
        case 2:
            if (hero->pos_x < 700.0 && hero->pos_y < 500.0) {
                *area = 1;
                return(true);
            }
            break;
    }
    return(false);
}

bool toggle_fullscreen(bool fullscreen, video_t* vid) {
    
    printf("toggeling fullscreen!\n");
    
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
	
    bool debug = false;                  // show object id and walls
    bool running = true;				// program running?
    const bool VSYNC = true;            // enable vertical sync
	const uint16_t FPS = 60;			// frames per second
    float dt = 1.0;                     // time step
    uint32_t time_start, time_end;		// to measure time per frame
    //Uint32 time;						// to measure time for debug
    uint64_t frame = 0;					// current frame
    bool fullscreen = false;            // fullscreen mode
    uint8_t lock_fullscreen_key = 0;
    uint8_t area = 2;                   // area / level to start the game with
    
    sound_t* snd = sound_init();
    
	video_t* vid = video_init(VSYNC);
	if (vid == NULL) {
		fprintf(stderr, "Initialization of sdl, ttf, or video failed!\n");
		return(true);
	}
    if (debug) {
        vid->show_ids = true;
        vid->show_walls = true;
    }
    if (VSYNC) {
        dt = 60.0 / vid->fps;
        printf("Vertical Syncronisation enabled. fps = %d\n", vid->fps);
    } else {
        dt = 60.0 / (float) FPS;
    }
    
	object_t* obj = on_init_objects(vid, dt, area);
    groups_t* grp = groups_init(obj);
    tasks_init(grp);
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
		
        
        
		while (SDL_PollEvent(&event)) {
			
			on_event(&event, keys);
		}
        
        if (lock_fullscreen_key) {
            
            lock_fullscreen_key--;
            
        } else if (keys[KEY_FULLSCREEN]) {
            
            lock_fullscreen_key = 30;
            fullscreen = toggle_fullscreen(fullscreen, vid);
        }
        
        if (keys[KEY_ESCAPE] || event.type == SDL_QUIT) {
            
            running = 0;
        }
        
		//time = SDL_GetTicks();
        
        /*printf("\n-------------------\n");
        printf("FRAME: %" PRId64 "\n", frame);
        printf("-------------------\n\n");*/
        
		on_loop(grp, snd, vbox, keys, frame, dt);
        
		//printf("time for on_loop: %d\n", SDL_GetTicks() - time);
		//time = SDL_GetTicks();
		on_render(grp, vid, dt);
		//printf("time for on_render: %d\n", SDL_GetTicks() - time);
		
        if (change_area(grp, &area)) {
            
            on_cleanup_area(grp, snd, vbox);
            obj = on_init_objects(vid, dt, area);
            grp = groups_init(obj);
            tasks_init(grp);
            vbox = verletbox_init(obj);
        }
        
		// ensure constant frame rate:
        if (!VSYNC) {
            
            time_end = SDL_GetTicks();
            
            if (1000 / FPS > time_end - time_start) {
                
                SDL_Delay(1000 / FPS - (time_end - time_start));
            }
        }
        
		frame++;
	}
    
    SDL_SaveBMP(vid->surface, "last_frame.bmp");

	
	on_cleanup(grp, vid, snd, vbox, keys);
 
	return(false);
}
 
int main(int argc, char** argv) {
	
	on_execute();
	
	return(0);
}
