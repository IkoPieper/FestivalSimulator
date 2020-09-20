#include "movements.h"

void movements(groups_t* grp, bool* keys, float dt) {
	
	object_t* obj_first = grp->obj_first;
	object_t* obj_hero = grp->obj_hero;
	object_t* obj_bg = grp->obj_bg;
	
	obj_hero->pos_x_old = obj_hero->pos_x;
	obj_hero->pos_y_old = obj_hero->pos_y;
	
	obj_bg->pos_x_old = obj_bg->pos_x;
	obj_bg->pos_y_old = obj_bg->pos_y;
	
	// hero movements first:
    obj_hero->acc_x = 0.0;
    obj_hero->acc_y = 0.0;
    movements_hero(obj_hero, keys);
    movements_accelerate(obj_hero, dt);
    movements_facing(obj_hero);
    
	// display / background movements:
	movements_background(obj_bg, obj_hero);
	
	if (obj_hero->pos_x != obj_hero->pos_x_old ||
		obj_hero->pos_y != obj_hero->pos_y_old) {
		obj_hero->has_moved = true;
	} else {
		obj_hero->has_moved = false;
	}
	
	// all objects:
	object_t* obj = obj_first;
	while (obj != NULL) {
		
		if (obj != obj_bg && obj != obj_hero) {
            
            // detect object movement:
            if (obj->can_move) {
				if (obj->pos_x != obj->pos_x_old ||
					obj->pos_y != obj->pos_y_old) {
					obj->has_moved = true;
				} else {
					obj->has_moved = false;
				}
			}
            
            // update old positions:
			obj->pos_x_old = obj->pos_x;
			obj->pos_y_old = obj->pos_y;
            
            if (obj->obj_carried_by != NULL) {
                
                movements_carried(obj, obj->obj_carried_by);
                
            } else if (obj->can_move) {
                
                movements_accelerate(obj, dt);
                movements_facing(obj_hero);
            }
            
			movements_screen_position(obj, obj_bg);
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

void movements_carried(object_t* obj, object_t* obj_host) {
    
    if (obj_host->anim_walk) {
        
        // position object according to direction the host object faces:
        switch (obj_host->anim->id) {
            case ANIMATION_WALK_NORTH:
            case ANIMATION_REST_NORTH:
                // center of host object:
                obj->pos_x = obj_host->pos_x + obj_host->surface->w / 2 - 
                    obj->surface->w / 2;
                // upper side of host object:
                obj->pos_y = obj_host->pos_y - 
                    obj->surface->h / 2;
                // shift collision area of object right above collision area of
                // host object:
                break;
            case ANIMATION_WALK_SOUTH:
            case ANIMATION_REST_SOUTH:
                // center of host object:
                obj->pos_x = obj_host->pos_x + obj_host->surface->w / 2 - 
                    obj->surface->w / 2;
                // lower side of host object:
                obj->pos_y = obj_host->pos_y + obj_host->surface->h - 
                    obj->surface->h / 2;
                break;
            case ANIMATION_WALK_WEST:
            case ANIMATION_REST_WEST:
                // left side of host object:
                obj->pos_x = obj_host->pos_x - 
                    obj->surface->w / 2;
                // lower third of host object:
                obj->pos_y = obj_host->pos_y + 2 * obj_host->surface->h / 3 - 
                    obj->surface->h / 2;
                break;
            case ANIMATION_WALK_EAST:
            case ANIMATION_REST_EAST:
                // right side of host object:
                obj->pos_x = obj_host->pos_x + obj_host->surface->w - 
                    obj->surface->w / 2;
                // lower third of host object:
                obj->pos_y = obj_host->pos_y + 2 * obj_host->surface->h / 3 - 
                    obj->surface->h / 2;
                break;
        }
        
        // position of the object's collision zone:
        /*switch (obj_host->anim->id) {
            case ANIMATION_WALK_NORTH:
            case ANIMATION_REST_NORTH:
                
                break;
            default:
                obj->wall->y_shift = obj_host->wall->y + 
                    obj_host->wall->y_shift + obj_host->wall->h;
        }*/
            
        /*// TODO: put in object_carried_start() or something called only once:
        obj->wall->x_shift = 0;
        // TODO: use lowest pixel of host and highest pixel of obj:
        obj->wall->y_shift = obj_host->wall->y + 
            obj_host->wall->y_shift + obj_host->wall->h;*/
    } else {
        
        // position object south of the host object:
        
    }
}

void movements_accelerate(object_t* obj, float dt) {
	
    const float vel_min = 0.1;
    
    if (obj->acc_x != 0.0 || obj->acc_y != 0.0) {
        
        // derive velocity from acceleration:
        obj->vel_x += obj->acc_x * dt;
        obj->vel_y += obj->acc_y * dt;
        
    } else {
        
        // set small velocities to zero:
        if (-vel_min < obj->vel_x && obj->vel_x < vel_min) {
            obj->vel_x = 0.0;
        }
        if (-vel_min < obj->vel_y && obj->vel_y < vel_min) {
            obj->vel_y = 0.0;
        }
    }
    
    if (!obj->disable_damping) {
        // add linear damping:
        obj->vel_x -= obj->damping * obj->vel_x * dt;
        obj->vel_y -= obj->damping * obj->vel_y * dt;
    }
    
    // limit high velocities:
    float vx = obj->vel_x;
    float vy = obj->vel_y;
    float norm = vx * vx + vy * vy;
    
    if (norm > 16.0) {
        norm = sqrtf(norm);
        obj->vel_x /= norm;
        obj->vel_y /= norm;
        obj->vel_x *= 4.0;
        obj->vel_y *= 4.0;
    }
    
    // derive position from velovity:
    obj->pos_x += obj->vel_x * dt;
    obj->pos_y += obj->vel_y * dt;
}

void movements_screen_position(object_t* obj, object_t* obj_bg) {
	
	// derive screen position:
	obj->scr_pos_x = (int32_t) obj->pos_x + (int32_t) obj_bg->scr_pos_x;
	obj->scr_pos_y = (int32_t) obj->pos_y + (int32_t) obj_bg->scr_pos_y;
}

void movements_facing(object_t* obj) {
    
    float vel_x = obj->vel_x;
    float vel_y = obj->vel_y;
    float vel_x_abs = fabsf(vel_x);
    float vel_y_abs = fabsf(vel_y);
    
    if (vel_x_abs > 0.0 || vel_y_abs > 0.0) {
        
        if (vel_y_abs > vel_x_abs) {
            if (vel_y < 0.0) {
                obj->facing = OBJECT_FACING_NORTH;
            } else {
                obj->facing = OBJECT_FACING_SOUTH;
            }
        } else {
            if (vel_x < 0.0) {
                obj->facing = OBJECT_FACING_WEST;
            } else {
                obj->facing = OBJECT_FACING_EAST;
            }
        }
    }
}
