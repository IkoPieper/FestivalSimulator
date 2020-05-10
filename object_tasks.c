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

void face(object_t* obj, object_t* obj_target) {
    
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
    object_animate(obj, 0);
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

bool (*get_task_function(uint64_t id))(task_t*, object_t*, bool*, uint64_t) {
    
    switch (id) {
        case TASK_FIND_BOB: return(&task_find_bob); break;
        case TASK_FIND_EVA: return(&task_find_eva); break;
    }
    
    return(NULL);
}

bool task_find_bob(task_t* tsk, object_t* obj, bool* keys, uint64_t frame) {
    
    if (tsk->step == 0) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
    
        if (fabsf(hero->pos_x - obj->pos_x) < 250 &&
            fabsf(hero->pos_y - obj->pos_y) < 250) {
        
            face(obj, hero);
            say(obj, 1, 170);
            
            tsk->step++;
            
            return(true);
        }
    }
    
    if (tsk->step == 1) {
        
        if (said(obj)) {
            
            object_t* hero = object_get(obj, OBJECT_HERO_ID);
            face(hero, obj);
            say_new(hero, "Ich liebe dich auch!", 100);
            
            list_t* lst = (list_t*) hero->mtr;
            lst = find_id(lst, METER_BEER);
            meter_t* mtr = (meter_t*) lst->entry;
            meter_update(mtr, mtr->value + 10);
            
            tsk->step++;
            
            return(true);
        }
    }
    
    if (tsk->step == 2) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
        
        if (said(hero)) {
            
            say_free(hero);
            
            move_on(obj);
            move_on(hero);
            
            tsk->step++;
            
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

bool task_find_eva(task_t* tsk, object_t* obj, bool* keys, uint64_t frame) {
    
    return(false);
    
}
