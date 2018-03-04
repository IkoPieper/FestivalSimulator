#include "movements.h"

void movements(object_t* obj, short* keys) {
	
	object_t* obj_first = NULL;
	object_t* obj_hero = NULL;
	object_t* obj_bg = NULL;
	
	obj_first = object_get_first(obj);
	obj_hero = object_get(obj, OBJECT_HERO_ID);
	obj_bg = object_get(obj, OBJECT_BACKGROUND_ID);
	
	obj_hero->pos_x_old = obj_hero->pos_x;
	obj_hero->pos_y_old = obj_hero->pos_y;
	obj_hero->scr_pos_x_old = obj_hero->scr_pos_x;
	obj_hero->scr_pos_y_old = obj_hero->scr_pos_y;
	
	obj_bg->pos_x_old = obj_bg->pos_x;
	obj_bg->pos_y_old = obj_bg->pos_y;
	obj_bg->scr_pos_x_old = obj_bg->scr_pos_x;
	obj_bg->scr_pos_y_old = obj_bg->scr_pos_y;
	
	// hero movements first:
	movements_hero(obj_hero, keys);
	movements_accelerate(obj_hero);
	movements_screen_position(obj_hero, obj_bg);
	
	// display / background movements:
	movements_background(obj_bg, obj_hero);
	//movements_screen_position(obj_hero, obj_bg);
	
	// all objects:
	obj = obj_first;
	while (obj != NULL) {
		
		if (obj->id != OBJECT_SURFDISPLAY_ID &&
			obj->id != OBJECT_BACKGROUND_ID &&
			obj->id != OBJECT_HERO_ID) {
				
			obj->pos_x_old = obj->pos_x;
			obj->pos_y_old = obj->pos_y;
			obj->scr_pos_x_old = obj->scr_pos_x;
			obj->scr_pos_y_old = obj->scr_pos_y;
			
			movements_accelerate(obj);
			movements_screen_position(obj, obj_bg);
		}
		
		obj = obj->next_object;
	}
	
}

void movements_hero(object_t* obj, short* keys) {
	
	short ax = 0;
	short ay = 0;
	float norm = 1.0;
	
	if (keys[SDLK_w] && obj->acc_y == 0) {
		ay -= 1;
	}
	if (keys[SDLK_s] && obj->acc_y == 0) {
		ay += 1;
	}
	if (keys[SDLK_a] && obj->acc_x == 0) {
		ax -= 1;
	}
	if (keys[SDLK_d] && obj->acc_x == 0) {
		ax += 1;
	}
	
	if (abs(ax) + abs(ay) > 1) {
		norm = 1.4142135;
	}
	
	obj->acc_x = obj->acc_abs * (float) ax / norm;
	obj->acc_y = obj->acc_abs * (float) ay / norm;
	
}

void movements_background(object_t* obj, object_t* obj_hero) {
	
	// default:
	obj->vel_x = 0.0;
	obj->vel_y = 0.0;
	
	// let the bg move instead of the hero if he reaches its screen
	// limits:
	if (obj_hero->scr_pos_x == obj_hero->max_scr_pos_x ||
		obj_hero->scr_pos_x == obj_hero->min_scr_pos_x) {
		obj->vel_x = -obj_hero->vel_x;
	}
	if (obj_hero->scr_pos_y == obj_hero->max_scr_pos_y ||
		obj_hero->scr_pos_y == obj_hero->min_scr_pos_y) {
		obj->vel_y = -obj_hero->vel_y;
	}
	
	// new background positions:
	obj->pos_x += obj->vel_x;
	obj->pos_y += obj->vel_y;
	obj->scr_pos_x = lroundf(obj->pos_x);
	obj->scr_pos_y = lroundf(obj->pos_y);
	
}

void movements_accelerate(object_t* obj) {
	
	const float vel_min = 0.1;
	
	// derive velocity from acceleration:
	if (!obj->vel_lock) {
		obj->vel_x += obj->acc_x;
		obj->vel_y += obj->acc_y;
	
		// add linear damping:
		obj->vel_x -= obj->damping * obj->vel_x;
		obj->vel_y -= obj->damping * obj->vel_y;
		// set small velocities to zero:
		if (-vel_min < obj->vel_x && obj->vel_x < vel_min) {
			obj->vel_x = 0.0;
		}
		if (-vel_min < obj->vel_y && obj->vel_y < vel_min) {
			obj->vel_y = 0.0;
		}
	}
	
	// derive position from velovity:
	obj->pos_x += obj->vel_x;
	obj->pos_y += obj->vel_y;
}

void movements_screen_position(object_t* obj, object_t* obj_bg) {
	
	// derive screen position:
	obj->scr_pos_x = lroundf(obj->pos_x + obj_bg->pos_x);
	obj->scr_pos_y = lroundf(obj->pos_y + obj_bg->pos_y);
	
	// object borders on screen:
	if (obj->scr_pos_x >= obj->max_scr_pos_x && obj->vel_x >= 0.0) {
		obj->scr_pos_x = obj->max_scr_pos_x;		  // limit object position
	} else if (obj->scr_pos_x <= obj->min_scr_pos_x && obj->vel_x <= 0.0) {
		obj->scr_pos_x = obj->min_scr_pos_x;
	}
	if (obj->scr_pos_y >= obj->max_scr_pos_y && obj->vel_y >= 0.0) {
		obj->scr_pos_y = obj->max_scr_pos_y;
	} else if (obj->scr_pos_y <= obj->min_scr_pos_y && obj->vel_y <= 0.0) {
		obj->scr_pos_y = obj->min_scr_pos_y;
	}
	
}
