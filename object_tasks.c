#include "object.h"

void say(object_t* obj, char* str, uint32_t duration) {
    
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
        
        object_t* bob = object_get(obj, 702);
    
        if (fabsf(obj->pos_x - bob->pos_x) < 250 &&
            fabsf(obj->pos_y - bob->pos_y) < 250) {
        
            face(bob, obj);
            say(bob, "Ich liebe dich du kleiner Knuddelbaer! Ach was ist das Leben schoen!!", 150);
            
            tsk->step++;
            
            return(true);
        }
    }
    
    if (tsk->step == 1) {
        
        object_t* bob = object_get(obj, 702);
        
        if (bob->txt_print == 0) {
            
            say_free(bob);
            
            face(obj, bob);
            say(obj, "Ich liebe dich auch!", 100);
            
            tsk->step++;
            
            return(true);
        }
    }
    
    if (tsk->step == 2) {
        
        if (obj->txt_print == 0) {
            
            say_free(obj);
            
            object_t* bob = object_get(obj, 702);
            move_on(bob);
            move_on(obj);
            
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
