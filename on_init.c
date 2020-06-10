#include "on_init.h"

sound_t* on_init_sound() {
    
    // Set up the audio stream
    int32_t result = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512);
    if (result < 0) {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        exit(-1);
    }

    result = Mix_AllocateChannels(8);
    if (result < 0) {
        fprintf(stderr, "Unable to allocate mixing channels: %s\n", SDL_GetError());
        exit(-1);
    }
    
    sound_t* snd = (sound_t*) malloc(sizeof(sound_t));
    
    snd = on_init_sound_samples(snd);
    snd = on_init_sound_songs(snd);
    
    return(snd);
}

// load audio samples as defined in sound.h:
sound_t* on_init_sound_samples(sound_t* snd) {
    
    
    return(snd);
}

// automatically load a list of music files and load the first 
// audiofile for playback:
sound_t* on_init_sound_songs(sound_t* snd) {
    
    snd->pos_x = 745.0;
    snd->pos_y = -200.0;
    snd->n = 0;
    
    DIR* hdl_dir;
	struct dirent* dir;
	char path[256];
	
	hdl_dir = opendir("songs");
	
    // count files:
    uint32_t n = 0;
    while ((dir = readdir(hdl_dir)) != NULL) {
        if (on_init_sound_supported(dir->d_name)) {
            n++;
        }
    }
    
    if (n == 0) {
        printf("No supported sound files found in songs folder!\n");
        snd->num_songs = n;
        snd->songs = NULL;
        return(snd);
    }
    
    snd->num_songs = n;
    snd->songs = (char**) malloc(n * sizeof(char*));
    
    rewinddir(hdl_dir);
    n = 0;
	while ((dir = readdir(hdl_dir)) != NULL) {
      
		if (on_init_sound_supported(dir->d_name)) {
            
			// build path to file:
			strncpy(path, "songs", 256);
			strncat(path, "/", 256);
			strncat(path, dir->d_name, 256);
            
            // save path:
            snd->songs[n] = (char*) malloc(256 * sizeof(char));
            strncpy(snd->songs[n], path, 256);
            
            n++;
		}
	}
    
    closedir(hdl_dir);
    
    for (n = 0; n < snd->num_songs; n++) {
        printf("song %d: %s\n", n, snd->songs[n]);
    }
    
    
    // load first song:
    if(!(snd->music = Mix_LoadMUS(snd->songs[0]))) {
		fprintf(stderr, "Error loading song %s!\n", snd->songs[0]);
    }
    
    Mix_PlayMusic(snd->music, 1);
    Mix_VolumeMusic(0);
    
    return(snd);
}

bool on_init_sound_supported(char* filename) {
    
    return(
        strcmp(filename, ".") && 
        strcmp(filename, "..") && (
            strstr(filename, ".wav") != NULL ||
            strstr(filename, ".WAV") != NULL ||
            strstr(filename, ".mp3") != NULL ||
            strstr(filename, ".MP3") != NULL ||
            strstr(filename, ".ogg") != NULL ||
            strstr(filename, ".OGG") != NULL ||
            strstr(filename, ".aiff") != NULL ||
            strstr(filename, ".AIFF") != NULL ||
            strstr(filename, ".flac") != NULL ||
            strstr(filename, ".FLAC") != NULL)
          );
}

video_t* on_init_video(bool VSYNC) {
    
    // init SDL:
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		return(NULL);
	}
	// init SDL fonts TTF:
	if(TTF_Init() == -1) {
		return(NULL);
	}
	
	/*if (SDL_VideoInit(NULL) != 0) {
		printf("Error initializing SDL video:  %s\n", SDL_GetError());
		return(NULL);
	}*/
    
    // TODO: free video stuff:
    video_t* vid = (video_t*) malloc(sizeof(video_t));
	
    int32_t w = 512;
    int32_t h = 448;
    
	// init SDL video and openGL:
    vid->window = SDL_CreateWindow("FestivalSimulator",
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             w, h,
                             SDL_WINDOW_OPENGL); // Windows needs this flag. Linux: 0

    
	//SDL_ShowCursor(SDL_DISABLE);
	
    vid->renderer = SDL_CreateRenderer(vid->window, -1, SDL_RENDERER_ACCELERATED);

    if (VSYNC) {
        SDL_GL_SetSwapInterval(1);
    }
    
    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	/*SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 3); // 3
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 3); // 3
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 2); // 2*/
    //SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    // Create an OpenGL context associated with the window:
    vid->glcontext = SDL_GL_CreateContext(vid->window);
    //SDL_GL_MakeCurrent(obj->window, obj->glcontext);
    // TODO: add glcontext to object and call SDL_GL_DeleteContext(glcontext)
    // and free if program is closed and object is freed
    // and maybe think about not using an object to store video stuff.
    // use a "video" structure instead
	
    
    vid->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h,
        32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);
 
	
    int32_t w_window, h_window;
    SDL_GetWindowSize(vid->window, &w_window, &h_window);
    float scale =  (float) h_window / (float) h;
    float w_viewport = scale * (float) w;
    float border = ((float) w_window - w_viewport) / 2.0;
    glViewport(border, 0, w_viewport, (float) h_window);
	
    
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, (float) w, (float) h, 0, 1, -1);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// create one texture name and tell opengl to use it
	GLuint textureid;
	glGenTextures(1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);
	vid->render_id = textureid;
	
	glEnable(GL_TEXTURE_2D);
	
	// these affect how this texture is drawn later on...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
    
    // Get frames per second:
    SDL_DisplayMode dsp_mode;
    SDL_GetCurrentDisplayMode(0, &dsp_mode);
    vid->fps = dsp_mode.refresh_rate;
    
    // include a nice font:
    if((vid->font = TTF_OpenFont("FreeSansBold.ttf", 13)) == NULL) {
		fprintf(stderr, "Error: Font FreeSansBold.ttf is missing!");
		return(NULL);
	}
    
	return(vid);
    
}

object_t* on_init_objects(video_t* vid, float dt) {
	
	// init objects:
	object_t* obj = NULL;
	
	obj = object_add(obj, OBJECT_BACKGROUND_ID);	// background
	
	if (on_init_background(obj, vid)) {
		object_clean_up(obj);
		return(NULL);
	}
	if (on_init_objects_config(obj, dt)) {
		object_clean_up(obj);
		return(NULL);
	}
	if (on_init_hero(obj, vid)) {
		object_clean_up(obj);
		return(NULL);
	}
    if (on_init_items(obj)) {
		object_clean_up(obj);
		return(NULL);
	}

	// init old positions:
	object_t* obj_tmp = object_get_first(obj);
	while (obj_tmp != NULL) {
		
		obj_tmp->pos_x_old = obj_tmp->pos_x;
		obj_tmp->pos_y_old = obj_tmp->pos_y;
		
		// get next object obj:
		obj_tmp = obj_tmp->next_object;
	}	
	
	return(object_get_first(obj));
}

bool on_init_background(object_t* obj, video_t* vid) {
	
	obj = object_get(obj, OBJECT_BACKGROUND_ID);
	
	obj->has_moved = 0;
	obj->mass = 99999999999.0;
	obj->damping = 1.0;
	
	SDL_Surface* surf_wall;
	if((surf_wall = surface_on_load("background_walls.bmp")) == NULL) {
		return(true);
	}
	if((obj->surface = surface_on_load("background.bmp")) == NULL) {
		return(true);
	}
	obj->wall = object_init_walls(surf_wall, obj->surface);
	
	obj->pos_x = 0.0;	// the background defines the positions
	obj->pos_y = 0.0;
	obj->scr_pos_x = -obj->surface->w / 2 + vid->surface->w / 2;
	obj->scr_pos_y = -obj->surface->h / 2 + vid->surface->h / 2;
	
	obj->min_scr_pos_x = -99999.0;
	obj->max_scr_pos_x = 99999.0;
	obj->min_scr_pos_y = -99999.0;
	obj->max_scr_pos_y = 99999.0;

	
	return(false);
}

bool on_init_hero(object_t* obj, video_t* vid) {
	
	// see objects/hero.txt for most of the initialization
	
	obj = object_get(obj, OBJECT_HERO_ID);
	object_t* obj_bg = object_get(obj, OBJECT_BACKGROUND_ID);
	
	// start position is middle of screen:
	obj->scr_pos_x = vid->surface->w / 2 - obj->surface->w / 2;
	obj->scr_pos_y = vid->surface->h / 2 - obj->surface->h / 2;
	obj->pos_x = obj_bg->surface->w / 2;
	obj->pos_y = obj_bg->surface->h / 2;
    //obj->pos_x = 1020;
	//obj->pos_y = 1220;
		
	// min/max screen positions:
	obj->min_scr_pos_x = (float) (0 + vid->surface->w / 3);
	obj->max_scr_pos_x = (float) (vid->surface->w 
		- vid->surface->w / 3 - obj->surface->w);
	obj->min_scr_pos_y = (float) (0 + vid->surface->h / 3);
	obj->max_scr_pos_y = (float) (vid->surface->h 
		- vid->surface->h / 3 - obj->surface->h);
    
    object_add_meter(obj, METER_BEER, METER_BEER,   10, 10);
    object_add_meter(obj, METER_MOOD, METER_MOOD,   10, 40);
    object_add_meter(obj, METER_URIN, METER_URIN,   10, 70);
    object_add_meter(obj, METER_POINTS, METER_POINTS, 10, 100);
    object_add_meter(obj, METER_ITEM, METER_ITEM, 10, 130);
    
	return(false);
}
	
bool on_init_objects_config(object_t* obj, float dt) {
	
	DIR* hdl_dir;
	struct dirent* dir;
	char path[100];
	
	hdl_dir = opendir("objects");
	
	while ((dir = readdir(hdl_dir)) != NULL) {
      
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..") &&
			strstr(dir->d_name, ".txt") != NULL) {

	
			// build path to file:
			strncpy(path, "objects", 100);
			strncat(path, "/", 100);
			strncat(path, dir->d_name, 100);
	
			configentry* data = conf_load_data(path);
			configentry* entry = data;

			if (data == NULL) {
				printf("Warning: Unable to load config file. Use Default Settings.\n");
				return(true);
			}
			
			obj = object_add(obj, 666666666);
			
			while (entry != NULL) {
			
				if        (strcmp(entry->key, "object") == 0) {
					entry = load_config_defaults(entry, path, obj);
                } else if (strcmp(entry->key, "item") == 0) {
					entry = load_config_item(entry, path, obj);
				} else if (strcmp(entry->key, "animation") == 0) {
					entry = load_config_animation(entry, path, obj, dt);
                } else if (strcmp(entry->key, "waypoints") == 0) {
					entry = load_config_waypoints(entry, path, obj, dt);
				} else if (strcmp(entry->key, "text") == 0) {
					entry = load_config_text(entry, obj);
				} 

			}
			
			conf_free_data(data);
		}
	}
    
    closedir(hdl_dir);
	
	return(false);
}

// fill existing item id lists with object pointers used for faster
// access in main loop:
bool on_init_items(object_t* obj) {
	
    obj = object_get_first(obj);
    
    while (obj != NULL) {
        
        if (obj->itm != NULL) {
            list_t* lst = obj->itm;
            lst = get_last(lst);
            
            while (lst != NULL) {
                object_t* obj_item = object_get(obj, lst->id);
                lst->entry = (void*) obj_item;
                lst->id = obj_item->itm_props->id;
                obj_item->disable_collision = true;
                obj_item->disable_render = true;
                obj_item->can_move = false;
                lst = lst->prev;
            }
        }
        
        obj = obj->next_object;
    }
    
	return(false);
}
