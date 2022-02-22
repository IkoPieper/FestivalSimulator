#include "on_loop.h"
#include "collisions.h"
#include "movements.h"
#include "animation.h"
#include "waypoints.h"

void on_loop(
    groups_t* grp, sound_t* snd, verletbox_t* vbox, 
    bool* keys, uint64_t frame, float dt) {
    
    //collisions(obj, vbox, dt);
    
    //movements(obj, keys, dt);
    
    on_loop_tasks(grp, keys, frame, dt);
    
    on_loop_items(grp->obj_hero, keys, frame, dt);
    
    on_loop_animations(grp, keys, frame, dt);
    
    on_loop_waypoints(grp, frame, dt);

    on_loop_sounds(grp, keys, snd, frame, dt);
    
    collisions(grp, vbox, dt);
    
    movements(grp, keys, dt);
}

// call all the task functions of the objects:
void on_loop_tasks(
    groups_t* grp, bool* keys, uint64_t frame, float dt) {
    
    object_t* obj;
    
    list_t* lst = grp->lst_have_tsk;
    
    while (lst != NULL) {
        
        obj = (object_t*) lst->entry;
        
        list_t* lst_tsk = get_first(obj->tsk);
        while (lst_tsk != NULL) {
            
            task_t* tsk = (task_t*) lst_tsk->entry;
            tsk->task_function(tsk, obj, grp, keys, frame, dt);
            
            lst_tsk = lst_tsk->next;
        }
        
        lst = lst->next;
    }
}

void on_loop_items(object_t* hero, bool* keys, uint64_t frame, float dt) {
    
    static bool released_key_shift = true;
    static bool released_key_ctrl = true;
    
    if (hero->itm != NULL) {
        
        // call item function of the item object. in other words:
        // use the selected item:
        object_t* obj = (object_t*) hero->itm->entry;
        obj->itm_props->item_function(obj, hero, keys, frame, dt);
        
        // item selection:
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

void on_loop_animations(
    groups_t* grp, bool* keys, uint64_t frame, float dt) {
    
    object_t* obj;
    
    list_t* lst = grp->lst_have_anim;
    
    while (lst != NULL) {
        
        obj = (object_t*) lst->entry;
        
        if (obj->anim_walk && obj->can_move) {
            
            uint32_t anim_id = 0;
            
            float vel_x = obj->vel_x;
            float vel_y = obj->vel_y;
            
            if (obj->obj_carried_by != NULL) {
                
                anim_id = ANIMATION_REST_SOUTH;
                
            } else {
                
                // select walk cycle animation:
                if (obj->id == OBJECT_HERO_ID) {
                    
                    anim_id = on_loop_get_animation_walk(obj, vel_x, vel_y);
                    
                } else {
                
                    animation_t* anim = (animation_t*) obj->anim->entry;
                    if (anim->time_active > 20.0) { // avoid fast changes
                        
                        anim_id = on_loop_get_animation_walk(obj, vel_x, vel_y);
                    }
                }
            }
        
            if (anim_id != 0) {
            
                if (anim_id != obj->anim->id) {
                    object_select_animation(obj, anim_id);
                }
            
                // set speed of animation depending on object velocity:
                float abs_vel = vel_x * vel_x + vel_y * vel_y;
                
                animation_t* anim = (animation_t*) obj->anim->entry;
                
                float delay_frames = 20.0 - 20.0 * abs_vel;
                delay_frames *= ((float) anim->delay_frames_wanted / 7.0);
                if (delay_frames < (float) anim->delay_frames_wanted) {
                    delay_frames = (float) anim->delay_frames_wanted;
                }
                
                anim->delay_frames = (uint32_t) delay_frames;
                if (anim->delay_frames < 1) {
                    anim->delay_frames = 1;
                }
            }
        }
        
        // animate:
        object_animate(obj, frame, dt);
        
        lst = lst->next;
    }
    
}

uint32_t on_loop_get_animation_walk(
    object_t* obj, float vel_x, float vel_y) {
    
    if (obj->obj_carries != NULL) {
        return(on_loop_get_animation_walk_carry(obj, vel_x, vel_y));
    }
    
    if (obj->itm != NULL) {
        
        object_t* obj_item = (object_t*) obj->itm->entry;
        
        if (obj_item->itm_props->step > 0) { // item is in use
            
            switch (obj_item->itm_props->id) {
                case ITEM_WATER_PISTOL:
                    return(
                        on_loop_get_animation_walk_pistol(obj, vel_x, vel_y));
            }
        }
    }
    
    return(on_loop_get_animation_walk_default(obj, vel_x, vel_y));
}

uint32_t on_loop_get_animation_walk_default(
    object_t* obj, float vel_x, float vel_y) {
    
    if (fabsf(vel_x) < 0.1 && fabsf(vel_y) < 0.1) {
        
        switch (obj->facing) {
            case OBJECT_FACING_NORTH:
                return(ANIMATION_REST_NORTH);
            case OBJECT_FACING_SOUTH:
                return(ANIMATION_REST_SOUTH);
            case OBJECT_FACING_WEST:
                return(ANIMATION_REST_WEST);
            case OBJECT_FACING_EAST:
                return(ANIMATION_REST_EAST);
        }
        
    } else {
        
        switch (obj->facing) {
            case OBJECT_FACING_NORTH:
                return(ANIMATION_WALK_NORTH);
            case OBJECT_FACING_SOUTH:
                return(ANIMATION_WALK_SOUTH);
            case OBJECT_FACING_WEST:
                return(ANIMATION_WALK_WEST);
            case OBJECT_FACING_EAST:
                return(ANIMATION_WALK_EAST);
        }
    }
    
    return(obj->anim->id);
}

uint32_t on_loop_get_animation_walk_carry(
    object_t* obj, float vel_x, float vel_y) {
    
    if (fabsf(vel_x) < 0.1 && fabsf(vel_y) < 0.1) {
        
        switch (obj->facing) {
            case OBJECT_FACING_NORTH:
                return(ANIMATION_CARRY_REST_NORTH);
            case OBJECT_FACING_SOUTH:
                return(ANIMATION_CARRY_REST_SOUTH);
            case OBJECT_FACING_WEST:
                return(ANIMATION_CARRY_REST_WEST);
            case OBJECT_FACING_EAST:
                return(ANIMATION_CARRY_REST_EAST);
        }
        
    } else {
        
        switch (obj->facing) {
            case OBJECT_FACING_NORTH:
                return(ANIMATION_CARRY_NORTH);
            case OBJECT_FACING_SOUTH:
                return(ANIMATION_CARRY_SOUTH);
            case OBJECT_FACING_WEST:
                return(ANIMATION_CARRY_WEST);
            case OBJECT_FACING_EAST:
                return(ANIMATION_CARRY_EAST);
        }
    }
    
    return(obj->anim->id);
}

uint32_t on_loop_get_animation_walk_pistol(
    object_t* obj, float vel_x, float vel_y) {
    
    if (fabsf(vel_x) < 0.1 && fabsf(vel_y) < 0.1) {
        
        switch (obj->facing) {
            case OBJECT_FACING_NORTH:
                return(ANIMATION_PISTOL_REST_NORTH);
            case OBJECT_FACING_SOUTH:
                return(ANIMATION_PISTOL_REST_SOUTH);
            case OBJECT_FACING_WEST:
                return(ANIMATION_PISTOL_REST_WEST);
            case OBJECT_FACING_EAST:
                return(ANIMATION_PISTOL_REST_EAST);
        }
        
    } else {
        
        switch (obj->facing) {
            case OBJECT_FACING_NORTH:
                return(ANIMATION_PISTOL_NORTH);
            case OBJECT_FACING_SOUTH:
                return(ANIMATION_PISTOL_SOUTH);
            case OBJECT_FACING_WEST:
                return(ANIMATION_PISTOL_WEST);
            case OBJECT_FACING_EAST:
                return(ANIMATION_PISTOL_EAST);
        }
    }
    
    return(obj->anim->id);
}

void on_loop_waypoints(groups_t* grp, uint64_t frame, float dt) {
    
    object_t* obj;
    
    list_t* lst = grp->lst_have_ways;
    
    while (lst != NULL) {
        
        obj = (object_t*) lst->entry;
        
        waypoints_t* ways = (waypoints_t*) obj->ways->entry;
    
        if (ways->active) {

            object_get_next_waypoint(obj, dt);
            object_aim_for_waypoint(obj);
        }
    
        lst = lst->next;
    }
    
}

void on_loop_sounds(
    groups_t* grp, bool* keys, sound_t* snd, uint64_t frame, float dt) {
    
    object_t* obj_hero = grp->obj_hero;
    
    // samples:
    object_t* obj = grp->obj_first;
    
    while (obj != NULL) {
        
        // walk sounds:
        if (obj->anim != NULL && obj->anim_walk && obj->anim->id < 5) {
            
            animation_t* anim = (animation_t*) obj->anim->entry;
            
            bool east_west = 
                obj->anim->id == ANIMATION_WALK_EAST ||
                obj->anim->id == ANIMATION_WALK_WEST;
            if (anim->surf_changed && 
                (anim->n == 0 || (east_west && anim->n == 2))) {
                
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
