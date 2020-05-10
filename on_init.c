#include "on_init.h"
 
object_t* on_init() {
	
	// init SDL:
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		return(NULL);
	}
	// init SDL fonts TTF:
	if(TTF_Init() == -1) {
		return(NULL);
	}
	
	if (SDL_VideoInit(NULL, 0) != 0) {
		printf("Error initializing SDL video:  %s\n", SDL_GetError());
		return(NULL);
	}

	// init objects:
	object_t* obj = NULL;
	
	obj = object_add(obj, OBJECT_SURFDISPLAY_ID);	// surf display
	obj = object_add(obj, OBJECT_BACKGROUND_ID);	// background
	
	if (on_init_surfdisplay(obj)) {	// inits video incl. openGL
		object_clean_up(obj);
		return(NULL);
	}
	if (on_init_background(obj)) {
		object_clean_up(obj);
		return(NULL);
	}
	if (on_init_objects(obj)) {
		object_clean_up(obj);
		return(NULL);
	}
	if (on_init_hero(obj)) {
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

bool on_init_surfdisplay(object_t* obj) {
	
	obj = object_get(obj, OBJECT_SURFDISPLAY_ID);
	obj->disable_collision = 1;
	obj->can_move = 0;
	
	// init SDL video and openGL:
	
	SDL_WM_SetCaption("Festival Simulator", "Festival Simulator");
	SDL_ShowCursor(SDL_DISABLE);
	
	//SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0); // disable vsync
	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 3);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 3);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 2);

	
	SDL_Surface* surf = NULL;
	if((surf = SDL_SetVideoMode(
		800, 600, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL)) == NULL) { 
			// add 0,0,32 for native resolution and SDL_FULLSCREEN for fullscreen
		return(true);
	}
	
	SDL_Surface* surf_tmp = NULL;
	
	if((surf_tmp = surface_on_load("surfdisplay.bmp")) == NULL) {
		return(true);
	}
	obj->surface = SDL_DisplayFormat(surf_tmp);
	SDL_FreeSurface(surf_tmp);
	
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);
 
	float ratio, scale, border, width;
	ratio = (float) surf->w / (float) surf->h;
	scale =  (float) surf->h / (float) obj->surface->h; // scale
	width = scale * (float) obj->surface->w;		// width of picture
	border = ((float) surf->w - width) / 2.0;		// width of border
	
	glViewport(border, 0, (int32_t) (width + 0.5), surf->h);
	
	//float scale2 = 1.5 * scale; // snes
	//float scale2 = 1.0 * scale; // 400x300
	float scale2 = 1.5 * scale;
	
	float ortho_w = (float) surf->w / (scale2 * 3.0 / 4.0 * ratio);
	float ortho_h = (float) surf->h / scale2;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, ortho_w, ortho_h, 0, 1, -1);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// create one texture name and tell opengl to use it
	GLuint textureid;
	glGenTextures(1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);
	obj->render_id = textureid;
	
	glEnable(GL_TEXTURE_2D);
	
	// these affect how this texture is drawn later on...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	return(false);
	
}

bool on_init_background(object_t* obj) {
	
	obj = object_get(obj, OBJECT_BACKGROUND_ID);
	object_t* obj_dsp = object_get(obj, OBJECT_SURFDISPLAY_ID);
	
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
	obj->scr_pos_x = -obj->surface->w / 2 + obj_dsp->surface->w / 2;
	obj->scr_pos_y = -obj->surface->h / 2 + obj_dsp->surface->h / 2;
	
	obj->min_scr_pos_x = -99999.0;
	obj->max_scr_pos_x = 99999.0;
	obj->min_scr_pos_y = -99999.0;
	obj->max_scr_pos_y = 99999.0;

	
	return(false);
}

bool on_init_hero(object_t* obj) {
	
	// see objects/hero.txt for most of the initialization
	
	obj = object_get(obj, OBJECT_HERO_ID);
	object_t* obj_bg = object_get(obj, OBJECT_BACKGROUND_ID);
	object_t* obj_dsp = object_get(obj, OBJECT_SURFDISPLAY_ID);
	
	// start position is middle of screen:
	obj->scr_pos_x = obj_dsp->surface->w / 2;
	obj->scr_pos_y = obj_dsp->surface->h / 2;
	obj->pos_x = obj->scr_pos_x - obj_bg->scr_pos_x;
	obj->pos_y = obj->scr_pos_y - obj_bg->scr_pos_y;
		
	// min/max screen positions:
	obj->min_scr_pos_x = (float) (0 + obj_dsp->surface->w / 5);
	obj->max_scr_pos_x = (float) (obj_dsp->surface->w 
		- obj_dsp->surface->w / 5 - obj->surface->w);
	obj->min_scr_pos_y = (float) (0 + obj_dsp->surface->h / 5);
	obj->max_scr_pos_y = (float) (obj_dsp->surface->h 
		- obj_dsp->surface->h / 5 - obj->surface->h);
    
    object_add_meter(obj, METER_BEER, METER_BEER,   10, 10);
    object_add_meter(obj, METER_MOOD, METER_MOOD,   10, 40);
    object_add_meter(obj, METER_URIN, METER_URIN,   10, 70);
    object_add_meter(obj, METER_POINTS, METER_POINTS, 10, 100);
    
	return(false);
}
	
	// waypoints:
	/*uint32_t num_ways = 5;
	object_add_waypoints(obj, 1, num_ways);
	* 
	obj->ways->pos_are_relative = 1;
	obj->ways->pos_x[0] = 0.0;
	obj->ways->pos_y[0] = 0.0;
	obj->ways->vel_abs[0] = 1.5;
	obj->ways->pos_x[1] = 4.0;
	obj->ways->pos_y[1] = 300.0;
	obj->ways->vel_abs[1] = 1.0;
	obj->ways->pos_x[2] = -20.0;
	obj->ways->pos_y[2] = 0.0;
	obj->ways->vel_abs[2] = 3.0;
	obj->ways->pos_x[3] = 20.0;
	obj->ways->pos_y[3] = 20.0;
	obj->ways->vel_abs[3] = 1.0;
	obj->ways->pos_x[4] = -20.0;
	obj->ways->pos_y[4] = 20.0;
	obj->ways->vel_abs[4] = 3.0;

	object_activate_waypoints(obj);*/
	
bool on_init_objects(object_t* obj) {
	
	DIR *hdl_dir;
	struct dirent *dir;
	char path[100];
	
	hdl_dir = opendir("objects");
	
	while ((dir = readdir(hdl_dir)) != NULL) {
      
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..") &&
			strstr(dir->d_name, ".txt") != NULL) {

	
			// build path to file:
			strncpy(path, "objects", 100);
			strncat(path, "/", 100);		// TODO: Windows compatible
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
				} else if (strcmp(entry->key, "animation") == 0) {
					entry = load_config_animation(entry, path, obj);
                } else if (strcmp(entry->key, "waypoints") == 0) {
					entry = load_config_waypoints(entry, path, obj);
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
