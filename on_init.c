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
	obj = object_add(obj, OBJECT_HERO_ID);			// hero
	obj = object_add(obj, OBJECT_SCORE_ID);			// score
	for (int n = 1; n <= 200; n++) {
		obj = object_add(obj, OBJECT_SCORE_ID + n); // buden
	}
	
	
	if (on_init_surfdisplay(obj) == 0) {	// inits video incl. openGL
		object_clean_up(obj);
		return(NULL);
	}
	if (on_init_background(obj) == 0) {
		object_clean_up(obj);
		return(NULL);
	}
	if (on_init_hero(obj) == 0) {
		object_clean_up(obj);
		return(NULL);
	}
	if (on_init_score(obj) == 0) {
		object_clean_up(obj);
		return(NULL);
	}
	if (on_init_buden(obj) == 0) {
		object_clean_up(obj);
		return(NULL);
	}
	if (on_init_objects(obj) == 0) {
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

short on_init_surfdisplay(object_t* obj) {
	
	obj = object_get(obj, OBJECT_SURFDISPLAY_ID);
	obj->disable_collision = 1;
	obj->can_move = 0;
	
	// init SDL video and openGL:
	
	SDL_WM_SetCaption("Festival Simulator", "Festival Simulator");
	SDL_ShowCursor(SDL_DISABLE);
	
	//SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0); // disable vsync
	
	SDL_Surface* surf = NULL;
	if((surf = SDL_SetVideoMode(
		800, 600, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL)) == NULL) { 
			// add 0,0,32 for native resolution and SDL_FULLSCREEN for fullscreen
		return(0);
	}
	
	if((obj->surface = surface_on_load("surfdisplay.bmp")) == NULL) {
		return(0);
	}
	
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);
 
	/*fprintf(stderr, "target: w: %d, h: %d\n", obj->surface->w, obj->surface->h);
	fprintf(stderr, "screen: w: %d, h: %d\n", surf->w, surf->h);*/
 
	float ratio, scale, border, width;
	ratio = (float) surf->w / (float) surf->h;
	scale =  (float) surf->h / (float) obj->surface->h; // scale
	width = scale * (float) obj->surface->w;		// width of picture
	border = ((float) surf->w - width) / 2.0;		// width of border
	/*fprintf(stderr, "scale: %f, width: %f, border: %f\n", scale, width, border);*/
	
	glViewport(border, 0, (int) (width + 0.5), surf->h);
	
	//float scale2 = 1.5 * scale; // snes
	//float scale2 = 1.0 * scale; // 400x300
	float scale2 = 1.5 * scale;
	
	float ortho_w = (float) surf->w / (scale2 * 3.0 / 4.0 * ratio);
	float ortho_h = (float) surf->h / scale2;
	
	/*fprintf(stderr, "ortho_w: %f, ortho_h: %f, scale2: %f\n", ortho_w, ortho_h, scale2);*/
	
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
	
	return(1);
	
}

short on_init_background(object_t* obj) {
	
	obj = object_get(obj, OBJECT_BACKGROUND_ID);
	object_t* obj_dsp = object_get(obj, OBJECT_SURFDISPLAY_ID);
	
	obj->mass = 99999999999.0;
	obj->damping = 1.0;
	
	SDL_Surface* surf_wall;
	if((surf_wall = surface_on_load("background_walls.bmp")) == NULL) {
		return(0);
	}
	if((obj->surface = surface_on_load("background.bmp")) == NULL) {
		return(0);
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

	
	return(1);
}

short on_init_hero(object_t* obj) {
	
	SDL_Surface* surf = NULL;
	
	obj = object_get(obj, OBJECT_HERO_ID);
	object_t* obj_bg = object_get(obj, OBJECT_BACKGROUND_ID);
	object_t* obj_dsp = object_get(obj, OBJECT_SURFDISPLAY_ID);
	
	// collision walls:
	SDL_Surface* surf_wall;
	if((surf_wall = surface_on_load("hero_walls.bmp")) == NULL) {
		return(0);
	}
	
	// default surface:
	if((surf = surface_on_load("hero.bmp")) == NULL) {
		return(0);
	}
	obj->surface = surf;
	
	obj->wall = object_init_walls(surf_wall, surf);
	
	// physics and collisions:
	obj->can_move = 1;
	obj->acc_abs = 0.5;	// determines the acceleration on key press
	obj->damping = 0.1;
	obj->mass = 1.0;
	
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
	

	// animation walk:
	
	// walk north:
	object_add_animation(obj, 1);
	obj->anim->delay_frames = 2;
	
	if((surf = surface_on_load("hero.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_n_1.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_n_2.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_n_3.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_n_4.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_n_5.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_n_6.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_n_7.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_n_8.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);

	
	// walk south:
	object_add_animation(obj, 2);
	obj->anim->delay_frames = 2;
	
	if((surf = surface_on_load("hero.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_s_1.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_s_2.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_s_3.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_s_4.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_s_5.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_s_6.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_s_7.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_s_8.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);


	// walk west:
	object_add_animation(obj, 3);
	obj->anim->delay_frames = 2;
	
	if((surf = surface_on_load("hero.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_w_1.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_w_2.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_w_3.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_w_4.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_w_5.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_w_6.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_w_7.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_w_8.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	
	// walk east:
	object_add_animation(obj, 4);
	obj->anim->delay_frames = 2;
	
	if((surf = surface_on_load("hero.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_e_1.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_e_2.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_e_3.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_e_4.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_e_5.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_e_6.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_e_7.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_e_8.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	
	// walk north west:
	object_add_animation(obj, 5);
	obj->anim->delay_frames = 2;
	
	if((surf = surface_on_load("hero.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_nw_1.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_nw_2.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_nw_3.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_nw_4.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_nw_5.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_nw_6.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_nw_7.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_nw_8.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	
	// walk north east:
	object_add_animation(obj, 6);
	obj->anim->delay_frames = 2;
	
	if((surf = surface_on_load("hero.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_ne_1.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_ne_2.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_ne_3.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_ne_4.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_ne_5.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_ne_6.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_ne_7.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_ne_8.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	
	// walk south west:
	object_add_animation(obj, 7);
	obj->anim->delay_frames = 2;
	
	if((surf = surface_on_load("hero.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_sw_1.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_sw_2.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_sw_3.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_sw_4.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_sw_5.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_sw_6.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_sw_7.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_sw_8.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	
	// walk south east:
	object_add_animation(obj, 8);
	obj->anim->delay_frames = 2;
	
	if((surf = surface_on_load("hero.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_se_1.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_se_2.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_se_3.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_se_4.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_se_5.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_se_6.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_se_7.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	if((surf = surface_on_load("hero_se_8.bmp")) == NULL) {
		return(0);
	}
	animation_add_surface(obj->anim, surf);
	
	
	return(1);
}

short on_init_score(object_t* obj) {
	
	obj = object_get(obj, OBJECT_SCORE_ID);
	object_t* obj_bg = object_get(obj, OBJECT_BACKGROUND_ID);
	
	obj->can_move = 1;
	obj->mass = 0.2;
	obj->damping = 0.03;
	
	obj->pos_x = 1112.0;
	obj->pos_y = 680.0;
	obj->scr_pos_x = obj->pos_x + obj_bg->scr_pos_x;
	obj->scr_pos_y = obj->pos_y + obj_bg->scr_pos_y;
	
	if((obj->font = TTF_OpenFont("FreeSansBold.ttf", 20)) == NULL) {
		return(0);
	}

	if((obj->surface = font_draw_text(obj->font, "0")) == NULL) {
		return(0);
	}
	
	obj->wall = object_init_walls(NULL, obj->surface);
	
	// waypoints:
	/*unsigned int num_ways = 5;
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
	
	return(1);
	
}

short on_init_buden(object_t* obj) {
	
	object_t* obj_bg = object_get(obj, OBJECT_BACKGROUND_ID);
	
	SDL_Surface* surf_wall;
	
	int x = 1;
	int y = 1;
	
	for (int n = 1; n <= 200; n++) {

		obj = object_get(obj, OBJECT_SCORE_ID + n);
		
		obj->can_move = 0;
		obj->mass = 99999999999.0;
		obj->damping = 1.0;
		
		obj->pos_x = x * 160;
		x++;
		
		if (obj->pos_x >= obj_bg->surface->w - 160) {
			x = 2;
			obj->pos_x = 160;
			y++;
		}
		
		obj->pos_y = y * 120;
		
		obj->scr_pos_x = obj->pos_x + obj_bg->scr_pos_x;
		obj->scr_pos_y = obj->pos_y + obj_bg->scr_pos_y;

		
		if((surf_wall = surface_on_load("bude_walls.bmp")) == NULL) {
			return(0);
		}
		if((obj->surface = surface_on_load("bude.bmp")) == NULL) {
			return(0);
		}
		obj->wall = object_init_walls(surf_wall, obj->surface);
		
	}
	
	return(1);
	
}

short on_init_objects(object_t* obj) {
	
	configentry *data;
	data = NULL;
	data = conf_load_data(data, "objects/stone.txt");

	if (data == NULL) {
		printf("Warning: Unable to load config file. Use Default Settings.\n");
		free(data);
		return(0);
	}
	
	obj = object_add(obj, conf_get_int(data, "object"));
	obj->pos_x = conf_get_double(data, "pos_x");
	obj->pos_y = conf_get_double(data, "pos_y");

	printf("\nid: %d\n\n", obj->id);
	printf("pos_x: %f\n", obj->pos_x);
	printf("pos_y: %f\n", obj->pos_y);

	conf_free_data(data);
	
	return(1);
}
