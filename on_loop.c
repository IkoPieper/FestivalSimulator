#include "on_loop.h"
#include "collisions.h"
#include "movements.h"
#include "animation.h"
#include "waypoints.h"

void on_loop(object_t* obj, sound_t* snd, 
    verletbox_t* vbox, bool* keys, uint64_t frame, float dt) {
	
	//collisions(obj, vbox, dt);
	
	//movements(obj, keys, dt);
	
    on_loop_tasks(obj, keys, frame, dt);
	
    on_loop_items(obj, keys, frame);
    
	on_loop_animations(obj, keys, frame, dt);
    
	on_loop_waypoints(obj, frame, dt);

    on_loop_sounds(obj, snd);
    
    collisions(obj, vbox, dt);
    
	movements(obj, keys, dt);
}

// call all the task functions of the objects:
void on_loop_tasks(object_t* obj, bool* keys, uint64_t frame, float dt) {
    
    obj = object_get_first(obj);
	while (obj != NULL) {
        
        if (obj->tsk != NULL) {
            
            list_t* lst = get_first(obj->tsk);
            while (lst != NULL) {
                
                task_t* tsk = (task_t*) lst->entry;
                tsk->task_function(tsk, obj, keys, frame, dt);
                
                lst = lst->next;
            }
        }
        
        obj = obj->next_object;
	}
}

void on_loop_items(object_t* obj, bool* keys, uint64_t frame) {
    
    object_t* hero = object_get(obj, OBJECT_HERO_ID);
    
    static bool released_key_shift = true;
    static bool released_key_ctrl = true;
    
    if (hero->itm != NULL) {
        
        // call item function of the item object. in other words:
        // use the selected item:
        object_t* obj = (object_t*) hero->itm->entry;
        obj->itm_props->item_function(obj, NULL, keys, frame);
        
        if (!keys[KEY_SHIFT]) {
            released_key_shift = true;
        }
        if (released_key_shift && keys[KEY_SHIFT]) {
            // select next item:
            if (hero->itm->next == NULL) {
                hero->itm = get_first(hero->itm);
            } else {
                hero->itm = hero->itm->next;
            }
            released_key_shift = false;
        }
        if (!keys[KEY_CTRL]) {
            released_key_ctrl = true;
        }
        if (released_key_ctrl && keys[KEY_CTRL]) {
            // select previous item:
            if (hero->itm->prev == NULL) {
                hero->itm = get_last(hero->itm);
            } else {
                hero->itm = hero->itm->prev;
            }
            released_key_ctrl = false;
        }
        
    }
    
    
}

void on_loop_animations(object_t* obj, bool* keys, 
    uint64_t frame, float dt) {
	
	obj = object_get_first(obj);
	
	while (obj != NULL) {
		
        if (obj->anim_walk && obj->can_move) {
        
            // calculate actual velocity:
            float vel_x = (obj->pos_x - obj->pos_x_old) / dt;
            float vel_y = (obj->pos_y - obj->pos_y_old) / dt;
            
            uint32_t anim_id = 0;
            
            // select walk cycle animation:
            if (obj->id == OBJECT_HERO_ID) {
                
                anim_id = on_loop_get_animation_walk_hero(obj, keys);
                
            } else {
            
                animation_t* anim = (animation_t*) obj->anim->entry;
                if (anim->time_active > 20.0) { // avoid fast changes
                    
                    anim_id = on_loop_get_animation_walk(
                        obj->anim->id, vel_x, vel_y);
                }
            }
        
            if (anim_id != 0) {
            
                if (anim_id != obj->anim->id) {
                    object_select_animation(obj, anim_id);
                }
            
                // set speed of animation depending on object velocity:
                float abs_vel = fabsf(vel_x * vel_x + vel_y * vel_y);
                int32_t delay_frames = (int32_t) 20 - 20 * abs_vel;
                if (delay_frames < 6) {
                    delay_frames = 6;
                }
                animation_t* anim = (animation_t*) obj->anim->entry;
                anim->delay_frames = delay_frames;
                
                // correct delay for monitor refresh rate:
                anim->delay_frames /= dt;
            }
        }
        
		// animate:
		if (obj->anim != NULL) {
			object_animate(obj, frame, dt);
		}
        
		obj = obj->next_object;
	}
	
}

uint32_t on_loop_get_animation_walk_hero(object_t* obj, bool* keys) {
    
    uint32_t anim_id = obj->anim->id;
    
    if (keys[KEY_UP]) {
        if (obj->itm->id == ITEM_WATER_PISTOL && keys[KEY_SPACE]) {
            return(9);
        } else {
            return(ANIMATION_WALK_NORTH);
        }
    }
    if (keys[KEY_DOWN]) {
        if (obj->itm->id == ITEM_WATER_PISTOL && keys[KEY_SPACE]) {
            return(10);
        } else {
            return(ANIMATION_WALK_SOUTH);
        }
    }
    if (keys[KEY_LEFT]) {
        return(ANIMATION_WALK_WEST);
    }
    if (keys[KEY_RIGHT]) {
        return(ANIMATION_WALK_EAST);
    }
    
    if (anim_id == ANIMATION_WALK_NORTH || anim_id == 9) {
        if (obj->itm->id == ITEM_WATER_PISTOL && keys[KEY_SPACE]) {
            return(11);
        } else {
            return(ANIMATION_REST_NORTH);
        }
    }
    if (anim_id == ANIMATION_REST_NORTH) {
        if (obj->itm->id == ITEM_WATER_PISTOL && keys[KEY_SPACE]) {
            return(11);
        } else {
            return(ANIMATION_REST_NORTH);
        }
    }
    if (anim_id == ANIMATION_WALK_SOUTH || anim_id == 10) {
        if (obj->itm->id == ITEM_WATER_PISTOL && keys[KEY_SPACE]) {
            return(12);
        } else {
            return(ANIMATION_REST_SOUTH);
        }
    }
    if (anim_id == ANIMATION_REST_SOUTH) {
        if (obj->itm->id == ITEM_WATER_PISTOL && keys[KEY_SPACE]) {
            return(12);
        } else {
            return(ANIMATION_REST_SOUTH);
        }
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
            if (vel_y < -0.8) {
                return(ANIMATION_WALK_NORTH);
            } else if (vel_y > 0.8) {
                return(ANIMATION_WALK_SOUTH);
            }
        } else {
            if (vel_x < -0.8) {
                return(ANIMATION_WALK_WEST);
            } else if (vel_x > 0.8) {
                return(ANIMATION_WALK_EAST);
            }
        }
    }
    
    return(anim_id);
}

void on_loop_waypoints(object_t* obj, uint64_t frame, float dt) {
	
	obj = object_get_first(obj);
	
	while (obj != NULL) {
        	
		if (obj->ways != NULL) {
            
            waypoints_t* ways = (waypoints_t*) obj->ways->entry;
        
            /*if (obj->id == 901 || obj->id == 902 || obj->id == OBJECT_BUS) {
                printf("%d: waypoint: %d\n", obj->id, obj->ways->id);
                printf("%d: active: %d\n", obj->id, ways->active);
                printf("%d: n: %d\n", obj->id, ways->n);
                printf("%d: frame: %d\n", obj->id, ways->frame);
                printf("%d: frames_wait: %d\n", obj->id, ways->frames_wait[ways->n]);
                printf("%d: frames_max: %d\n\n", obj->id, ways->frames_max[ways->n]);
            }*/
            
        
            if (ways->active && !obj->vel_lock) {

                object_get_next_waypoint(obj, dt);
                object_aim_for_waypoint(obj);
            }
		}
	
		obj = obj->next_object;
	}
	
}

void on_loop_sounds(object_t* obj, sound_t* snd) {
    
    object_t* obj_hero = object_get(obj, OBJECT_HERO_ID);
    
    /* if (sound collision play finished) {
    
        obj = object_get_first(obj);
        
        while (obj != NULL) {
        
            if (obj->col != NULL) {
            
                // Play sound
                
                break;
            }
        
            obj = obj->next_object;
        }
    }*/
    
    // stage music:
    if (snd->num_songs > 0) {
        
        // set volume according to distance to sound source:
        float dist_x = obj_hero->pos_x - snd->pos_x;
        float dist_y = obj_hero->pos_y - snd->pos_y;
        float dist = sqrtf(dist_x * dist_x + dist_y * dist_y);
        if (dist < 100.0) {
            dist = 100.0;
        }
        int32_t volume = 100.0 * (float) MIX_MAX_VOLUME / dist;
        if (volume < 1) {
            volume = 1;
        }
        
        Mix_VolumeMusic(volume);
        
        // load next song in loop if finished playing:
        if (!Mix_PlayingMusic()) {
            
            Mix_FreeMusic(snd->music);
            
            snd->n++;
            if (snd->n == snd->num_songs) {
                snd->n = 0;
            }
            if(!(snd->music = Mix_LoadMUS(snd->songs[snd->n]))) {
                fprintf(stderr, "Error loading song %s!\n", snd->songs[snd->n]);
            }
            Mix_PlayMusic(snd->music, 1);
        }
    }
}
