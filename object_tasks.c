#include "object.h"

void say(object_t* obj, uint32_t id, uint32_t duration) {
    
    object_select_text(obj, id);
    text_t* txt = (text_t*) obj->txt->entry;
    txt->duration = duration;
    obj->txt_print = 1;
}

bool said(object_t* obj) {
    
    return(obj->txt_print == 0);
}

// if calling say_new, you have to call say_free at a later step:
void say_new(object_t* obj, char* str, uint32_t duration) {
    
    object_add_text(obj, 0);
    text_t* txt = (text_t*) obj->txt->entry;
    text_add_string(txt, str);
    txt->duration = duration;
    obj->txt_print = 1;
}

void say_free(object_t* obj) {
    
    object_remove_text(obj, 0);
}

void face(object_t* obj, object_t* obj_target, float dt) {
    
    if (obj->id == OBJECT_HERO_ID) {
        obj->can_move = false;
    } else {
        waypoints_t* ways = (waypoints_t*) obj->ways->entry;
        ways->active = false;
    }
    obj->anim_walk = false;
    
    float x = obj_target->pos_x;
    float y = obj_target->pos_y;
    object_select_animation_target(obj, x, y);
    object_animate(obj, 0, dt);
}

void move_on(object_t* obj) {
    
    if (obj->id == OBJECT_HERO_ID) {
        obj->can_move = true;
    } else {
        waypoints_t* ways = obj->ways->entry;
        ways->active = true;
    }
    obj->anim_walk = true;
}

void drink_beer(object_t* obj, int16_t value) {
    
    list_t* lst = (list_t*) obj->mtr;
    lst = find_id(lst, METER_BEER);
    meter_t* mtr = (meter_t*) lst->entry;
    meter_update(mtr, mtr->value + value);
}

void change_mood(object_t* obj, int16_t value) {
    
    list_t* lst = (list_t*) obj->mtr;
    lst = find_id(lst, METER_MOOD);
    meter_t* mtr = (meter_t*) lst->entry;
    meter_update(mtr, mtr->value + value);
}

void start_waypoints(object_t* obj, uint32_t id) {
    
    object_select_waypoints(obj, id);
    waypoints_t* ways = (waypoints_t*) obj->ways->entry;
    ways->n = 0;
    ways->active = true;
}

bool waypoints_finished(object_t* obj) {
    
    waypoints_t* ways = (waypoints_t*) obj->ways->entry;
    
    return(!ways->active);
}

bool (*get_task_function(uint32_t id))(task_t*, object_t*, bool*, 
    uint64_t, float) {
    
    switch (id) {
        case TASK_FIND_BOB: return(&task_find_bob); break;
        case TASK_FIND_EVA: return(&task_find_eva); break;
        case TASK_SECURITY_FENCE: return(&task_security_fence); break;
    }
    
    return(NULL);
}

bool task_find_bob(task_t* tsk, object_t* obj, bool* keys, 
    uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
    
        if (fabsf(hero->pos_x - obj->pos_x) < 100.0 &&
            fabsf(hero->pos_y - obj->pos_y) < 100.0) {
        
            face(obj, hero, dt);
            say(obj, 1, 150);
            
            tsk->step = 1;
            
            return(true);
        }
    }
    
    if (tsk->step == 1) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
        
        if (keys[KEY_SPACE] && hero->itm != NULL) {
                
            object_t* obj_itm = (object_t*) hero->itm->entry;
            if (obj_itm->itm_props->id == ITEM_MONEY) {
                
                face(hero, obj, dt);
                say_new(hero, "Hier! Nimm mein Geld!", 60);
                
                tsk->step = 2;
                
                return(true);
            }
        }
        
        if (said(obj)) {
            
            move_on(obj);
            
            tsk->step = 0;
            
            return(true);
        }
    }
    
    if (tsk->step == 2) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
        
        if (said(hero)) {
            
            say_free(hero);
            
            say(obj, 2, 50);
            
            move_on(obj);
            move_on(hero);
            
            drink_beer(hero, 10);
            change_mood(hero, 10);
            
            tsk->step = 3;
            
            return(true);
        }
    }
    
    if (tsk->step >= 3) {
        
        // use step as counter:
        tsk->step++;
        
        if (tsk->step == 300) {
            tsk->step = 0;
            
            return(true);
        }
        
    }
    
    return(false);
    
}

bool task_find_eva(task_t* tsk, object_t* obj, bool* keys, 
    uint64_t frame, float dt) {
    
    return(false);
    
}

bool task_security_fence(task_t* tsk, object_t* obj, bool* keys, 
    uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
    
        if (hero->pos_x < 350.0 && hero->pos_y < 450.0) {
        
            start_waypoints(obj, 1);
            
            tsk->step++;
            
            return(true);
        }
    }
    
    if (tsk->step == 1) {
        
        if (waypoints_finished(obj)) {
            
            say(obj, 1, 150);
            
            obj->can_move = false;
            
            tsk->step++;
            
            return(true);
        }
    }
    
    if (tsk->step == 2 || tsk->step == 1) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
        
        if (hero->pos_x > 350.0 || hero->pos_y > 450.0) {
            
            obj->can_move = true;
            
            start_waypoints(obj, 2);
            
            tsk->step = 0;
            
            return(true);
        }
        
        if (keys[KEY_SPACE] && hero->itm != NULL) {
                
            object_t* obj_itm = (object_t*) hero->itm->entry;
            if (obj_itm->itm_props->id == ITEM_RED_STONE) {
            
                say(obj, 2, 250);
                
                obj->can_move = true;
                
                start_waypoints(obj, 3);
                
                tsk->step = 3;
                
                return(true);
            }
        }
    }
    
    if (tsk->step == 3) {
        
        if (waypoints_finished(obj)) {
            
            tsk->step = 4;
            
            return(true);
        }
    }
    
    if (tsk->step == 4) {
        
        static uint16_t frame = 0;
        static float vel_x;
        static float vel_y;
        
        if (obj->col != NULL && frame == 0) {
            
            // reduce heros mood on collision:
            if (find_id(obj->col, OBJECT_HERO_ID) != NULL) {
                object_t* hero = object_get(obj, OBJECT_HERO_ID);
                change_mood(hero, -10);
            }
            
            collision_t* col = (collision_t*) obj->col->entry;
            
            uint32_t rdm = rand();
            
            // start to go away from collision in 90° angle:
            if (rdm > RAND_MAX / 4) {
                vel_x = col->c_y;
                vel_y = -col->c_x;
            } else if (rdm > RAND_MAX / 8) {
                vel_x = -col->c_y;
                vel_y = col->c_x;
            } else if (rdm > RAND_MAX / 16) {
                vel_x = -col->c_x;
                vel_y = -col->c_y;
            } else {
                vel_x = col->c_x;
                vel_y = col->c_y;
            }
            
            vel_x *= 2.0;
            vel_y *= 2.0;
            
            obj->vel_x = vel_x;
            obj->vel_y = vel_y;
            
            frame = (uint16_t) (30.0 / dt);
            
        } else if (frame > 0.0) {
            
            // go away from collision:
            obj->vel_x = vel_x;
            obj->vel_y = vel_y;
            
            obj->vel_lock = true;
            
            frame--;
            
        } else {
                
            // follow hero:
            object_t* hero = object_get(obj, OBJECT_HERO_ID);
            
            float vel_x_wanted = hero->pos_x - obj->pos_x;
            float vel_y_wanted = hero->pos_y - obj->pos_y;
            float norm = sqrtf(vel_x_wanted * vel_x_wanted + vel_y_wanted * vel_y_wanted);
            vel_x_wanted /= norm;
            vel_y_wanted /= norm;
            
            vel_x_wanted *= 2.0;
            vel_y_wanted *= 2.0;
            
            obj->vel_x = vel_x_wanted;
            obj->vel_y = vel_y_wanted;
        }
        
        return(true);
    }
    
    return(false);
}
