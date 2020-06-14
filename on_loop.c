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

    on_loop_sounds(obj, keys, snd, frame, dt);
    
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
            
        
            if (ways->active) {

                object_get_next_waypoint(obj, dt);
                object_aim_for_waypoint(obj);
            }
		}
	
		obj = obj->next_object;
	}
	
}

void on_loop_sounds(
    object_t* obj, bool* keys, sound_t* snd, uint64_t frame, float dt) {
    
    object_t* obj_hero = object_get(obj, OBJECT_HERO_ID);
    
    // samples:
    object_t* obj_first = object_get_first(obj);
    obj = obj_first;
    while (obj != NULL) {
        
        // walk sounds:
        if (obj->anim != NULL && obj->anim_walk && obj->anim->id < 5) {
            
            animation_t* anim = (animation_t*) obj->anim->entry;
            
            bool east_west = 
                obj->anim->id == ANIMATION_WALK_EAST ||
                obj->anim->id == ANIMATION_WALK_WEST;
            if (anim->surf_changed && 
                (anim->n == 0 || (east_west && anim->n == 2))) {
                
                printf("SOUND obj->id: %d\n", obj->id);
                
                if (obj->id == OBJECT_HERO_ID) {
                    
                    sound_play_sample(
                        snd, SOUND_STEP, 
                        CHANNEL_HERO_STEP);
                } else {
                    
                    float dist_x = obj->pos_x - obj_hero->pos_x;
                    float dist_y = obj->pos_y - obj_hero->pos_y;
                    
                    sound_play_sample_distance(
                        snd, SOUND_STEP, 
                        dist_x, dist_y);
                }
            }
        }
        
        // collisions:
        if (obj->col_sample_timer > 0) {
            
            obj->col_sample_timer--;
        }
        
        if (obj->col != NULL) {
            
            list_t* lst = get_first(obj->col);
            while (lst != NULL) {
                
                collision_t* col = (collision_t*) lst->entry;
                
                object_t* obj_tmp;
                if (col->partner->can_move) {
                    obj_tmp = col->partner;
                } else {
                    obj_tmp = obj;
                }
                
                if (obj_tmp->col_sample_timer < 1) {
                    
                    if (obj_tmp->id == OBJECT_HERO_ID ||
                        obj->id == OBJECT_HERO_ID) {
                            
                        sound_play_sample(
                            snd, SOUND_COLLISION, 
                            CHANNEL_HERO_COLLISION);
                    } else {
                    
                        float dist_x = obj_tmp->pos_x - obj_hero->pos_x;
                        float dist_y = obj_tmp->pos_y - obj_hero->pos_y;
                        
                        printf("SOUND played for obj_tmp->id: %d\n", obj_tmp->id);
                        
                        sound_play_sample_distance(
                            snd, SOUND_COLLISION, 
                            dist_x, dist_y);
                    }
                        
                    obj_tmp->col_sample_timer = (uint32_t) (20.0 / dt);
                }
                lst = lst->next;
            }
        }
        
        
        obj = obj->next_object;
    }
    
    // hero items:
    if (obj_hero->itm != NULL) { 
        if (keys[KEY_SPACE]) {
            if (obj_hero->itm->id == ITEM_WATER_PISTOL) {
                
                sound_play_sample(
                    snd, SOUND_WATER_PISTOL, 
                    CHANNEL_HERO_ITEM);
            }
        } else {
            Mix_FadeOutChannel(CHANNEL_HERO_ITEM, 200);
        }
    }
    
    // stage music:
    sound_music_loop(snd);
    sound_music_volume_distance(snd, obj_hero->pos_x, obj_hero->pos_y);
}
