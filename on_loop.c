#include "on_loop.h"
#include "collisions.h"
#include "movements.h"
#include "animation.h"
#include "waypoints.h"

void on_loop(object_t* obj, verletbox_t* vbox, bool* keys, uint64_t frame) {
	
	//Uint32 time;
	
	//time = SDL_GetTicks();
	collisions(obj, vbox);
	//printf("time for collisions: %d\n", SDL_GetTicks() - time);
	
	//time = SDL_GetTicks();
	movements(obj, keys);
	//printf("time for movements: %d\n", SDL_GetTicks() - time);
	
    on_loop_tasks(obj, keys, frame);
    
    on_loop_items(obj, keys, frame);
    
	//time = SDL_GetTicks();
	on_loop_animations(obj, keys, frame);
	
	on_loop_waypoints(obj, frame);
	//printf("time for animations and waypoints: %d\n", SDL_GetTicks() - time);

}

// call all the task functions of the objects:
void on_loop_tasks(object_t* obj, bool* keys, uint64_t frame) {
    
    obj = object_get_first(obj);
	while (obj != NULL) {
        
        if (obj->tsk != NULL) {
            
            list_t* lst = get_first(obj->tsk);
            while (lst != NULL) {
                
                task_t* tsk = (task_t*) lst->entry;
                tsk->task_function(tsk, obj, keys, frame);
                
                lst = lst->next;
            }
        }
        
        obj = obj->next_object;
	}
}

void on_loop_items(object_t* obj, bool* keys, uint64_t frame) {
    
    object_t* hero = object_get(obj, OBJECT_HERO_ID);
    
    static uint16_t frames_wait = 0;
    frames_wait++;
    
    if (hero->itm != NULL) {
        
        if (keys[KEY_SPACE]) {
            // call item function of the item object. in other words:
            // use the selected item:
            object_t* obj = (object_t*) hero->itm->entry;
            obj->itm_props->item_function(obj, NULL, keys, frame);
        }
        if (frames_wait > 15 && keys[KEY_SHIFT]) {
            // select next item:
            if (hero->itm->next == NULL) {
                hero->itm = get_first(hero->itm);
            } else {
                hero->itm = hero->itm->next;
            }
            frames_wait = 0;
        }
        if (frames_wait > 15 && keys[KEY_CTRL]) {
            // select previous item:
            if (hero->itm->prev == NULL) {
                hero->itm = get_last(hero->itm);
            } else {
                hero->itm = hero->itm->prev;
            }
            frames_wait = 0;
        }
        
    }
}

void on_loop_animations(object_t* obj, bool* keys, uint64_t frame) {
	
	obj = object_get_first(obj);
	
	while (obj != NULL) {
		
        // calculate actual velocity:
        float vel_x = obj->pos_x - obj->pos_x_old;
        float vel_y = obj->pos_y - obj->pos_y_old;
        
        // select walk cycle animation:
        uint32_t anim_id = 0;
        if (obj->id == OBJECT_HERO_ID) {
            
            if (obj->can_move) {
                anim_id = on_loop_get_animation_walk_hero(
                    obj->anim->id, keys);
            }
            
        } else {
            
            if (obj->anim_walk) {
                anim_id = on_loop_get_animation_walk(
                    obj->anim->id, vel_x, vel_y);
            }
        }
        
        if (anim_id != 0) {
            
            if (anim_id != obj->anim->id) {
                object_select_animation(obj, anim_id);
            }
        
            // set speed of animation depending on object velocity:
            float abs_vel = fabsf(
                obj->vel_x * obj->vel_x + obj->vel_y * obj->vel_y);
            int32_t delay_frames = (int32_t) 20 - 20 * abs_vel;
            if (delay_frames < 5) {
                delay_frames = 5;
            }
            animation_t* anim = (animation_t*) obj->anim->entry;
            anim->delay_frames = delay_frames;
                    
        }
        
		// animate:
		if (obj->anim != NULL) {
			object_animate(obj, frame);
		}
        
		obj = obj->next_object;
	}
	
}

uint32_t on_loop_get_animation_walk_hero(uint32_t anim_id, bool* keys) {
    
    if (keys[KEY_UP]) {
        return(ANIMATION_WALK_NORTH);
    }
    if (keys[KEY_DOWN]) {
        return(ANIMATION_WALK_SOUTH);
    }
    if (keys[KEY_LEFT]) {
        return(ANIMATION_WALK_WEST);
    }
    if (keys[KEY_RIGHT]) {
        return(ANIMATION_WALK_EAST);
    }
    
    if (anim_id == ANIMATION_WALK_NORTH) {
        return(ANIMATION_REST_NORTH);
    }
    if (anim_id == ANIMATION_WALK_SOUTH) {
        return(ANIMATION_REST_SOUTH);
    }
    if (anim_id == ANIMATION_WALK_WEST) {
        return(ANIMATION_REST_WEST);
    }
    if (anim_id == ANIMATION_WALK_EAST) {
        return(ANIMATION_REST_EAST);
    }
    
    return(0);
}

uint32_t on_loop_get_animation_walk(
    uint32_t anim_id, float vel_x, float vel_y) {
    
    if (fabsf(vel_x) < 0.1 && fabsf(vel_y) < 0.1) {
        
        // start a stop animation:
        if (anim_id == ANIMATION_WALK_NORTH) {
            return(ANIMATION_REST_NORTH);
        }
        if (anim_id == ANIMATION_WALK_SOUTH) {
            return(ANIMATION_REST_SOUTH);
        }
        if (anim_id == ANIMATION_WALK_WEST) {
            return(ANIMATION_REST_WEST);
        }
        if (anim_id == ANIMATION_WALK_EAST) {
            return(ANIMATION_REST_EAST);
        }
        
    } else {
        
        // start a walk animation:
        if (fabsf(vel_y) > fabsf(vel_x)) {
            if (vel_y < 0) {
                return(ANIMATION_WALK_NORTH);
            } else {
                return(ANIMATION_WALK_SOUTH);
            }
        } else {
            if (vel_x < 0) {
                return(ANIMATION_WALK_WEST);
            } else {
                return(ANIMATION_WALK_EAST);
            }
        }
    }
    
    return(0);
}

void on_loop_waypoints(object_t* obj, uint64_t frame) {
	
	obj = object_get_first(obj);
	
	while (obj != NULL) {
        	
		if (obj->ways != NULL) {
            
            waypoints_t* ways = (waypoints_t*) obj->ways->entry;
        
            if (ways->active && !obj->vel_lock) {

                object_get_next_waypoint(obj);
                
            }
		}
	
		obj = obj->next_object;
	}
	
}
