#include "movements.h"

void movements(object_t* obj, bool* keys, float dt) {
	
	object_t* obj_first = NULL;
	object_t* obj_hero = NULL;
	object_t* obj_bg = NULL;
	
	obj_first = object_get_first(obj);
	obj_hero = object_get(obj, OBJECT_HERO_ID);
	obj_bg = object_get(obj, OBJECT_BACKGROUND_ID);
    
	obj_hero->pos_x_old = obj_hero->pos_x;
	obj_hero->pos_y_old = obj_hero->pos_y;
	
	obj_bg->pos_x_old = obj_bg->pos_x;
	obj_bg->pos_y_old = obj_bg->pos_y;
	
	// hero movements first:
	movements_hero(obj_hero, keys);
	movements_accelerate(obj_hero, dt);
    
	// display / background movements:
	movements_background(obj_bg, obj_hero);
	
	if (obj_hero->pos_x != obj_hero->pos_x_old ||
		obj_hero->pos_y != obj_hero->pos_y_old) {
		obj_hero->has_moved = 1;
	} else {
		obj_hero->has_moved = 0;
	}
	
	
	// all objects:
	obj = obj_first;
	while (obj != NULL) {
		
		if (obj->id != OBJECT_BACKGROUND_ID &&
			obj->id != OBJECT_HERO_ID) {
				
			obj->pos_x_old = obj->pos_x;
			obj->pos_y_old = obj->pos_y;
			
			movements_accelerate(obj, dt);
			movements_screen_position(obj, obj_bg);
			
			if (obj->can_move) {
				if (obj->pos_x != obj->pos_x_old ||
					obj->pos_y != obj->pos_y_old) {
					obj->has_moved = 1;
				} else {
					obj->has_moved = 0;
				}
			}
				
		}
		
		obj = obj->next_object;
	}
}

void movements_hero(object_t* obj, bool* keys) {
	
	int8_t ax = 0;
	int8_t ay = 0;
	float norm = 1.0;
	
	if (keys[KEY_UP]) {
		ay -= 1;
	}
	if (keys[KEY_DOWN]) {
		ay += 1;
	}
	if (keys[KEY_LEFT]) {
		ax -= 1;
	}
	if (keys[KEY_RIGHT]) {
		ax += 1;
	}
	
	if (abs(ax) + abs(ay) > 1) {
		norm = 1.4142135;
	}
	
	obj->acc_x = obj->acc_abs * (float) ax / norm;
	obj->acc_y = obj->acc_abs * (float) ay / norm;
}

void movements_background(object_t* obj, object_t* obj_hero) {
	
    // new background screen positions:
    obj->scr_pos_x = -obj_hero->pos_x + obj_hero->scr_pos_x;
    obj->scr_pos_y = -obj_hero->pos_y + obj_hero->scr_pos_y;
}

void movements_accelerate(object_t* obj, float dt) {
	
	if (obj->can_move) {
	
		const float vel_min = 0.1 * dt;
		
		// derive velocity from acceleration:
		if (!obj->vel_lock) {
			obj->vel_x += obj->acc_x * dt;
			obj->vel_y += obj->acc_y * dt;
		
			// add linear damping:
			obj->vel_x -= obj->damping * obj->vel_x * dt;
			obj->vel_y -= obj->damping * obj->vel_y * dt;
			// set small velocities to zero:
			if (-vel_min < obj->vel_x && obj->vel_x < vel_min) {
				obj->vel_x = 0.0;
			}
			if (-vel_min < obj->vel_y && obj->vel_y < vel_min) {
				obj->vel_y = 0.0;
			}
		}
		
		// derive position from velovity:
		obj->pos_x += obj->vel_x * dt;
		obj->pos_y += obj->vel_y * dt;
	}
}

void movements_screen_position(object_t* obj, object_t* obj_bg) {
	
	// derive screen position:
	obj->scr_pos_x = (int32_t) obj->pos_x + (int32_t) obj_bg->scr_pos_x;
	obj->scr_pos_y = (int32_t) obj->pos_y + (int32_t) obj_bg->scr_pos_y;
}
