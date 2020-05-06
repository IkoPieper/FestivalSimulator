#include "object.h"

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
    
        if (fabsf(obj->pos_x - bob->pos_x) < 150 &&
            fabsf(obj->pos_y - bob->pos_y) < 150) {
        
            object_add_text(bob, 0);
            text_t* txt = (text_t*) bob->txt->entry;
            text_add_string(txt, "Ich liebe dich!");
            bob->txt_print = 1;
            
            waypoints_t* ways = (waypoints_t*) bob->ways->entry;
            ways->active = false;
            bob->anim_walk = false;
            object_select_animation_target(bob, obj->pos_x, obj->pos_y);
            object_animate(bob, frame);
            
            obj->can_move = false;
            obj->anim_walk = false;
            object_select_animation_target(obj, bob->pos_x, bob->pos_y);
            object_animate(obj, frame);
            
            tsk->step++;
            
            return(true);
        }
    }
    
    if (tsk->step == 1) {
        
        object_t* bob = object_get(obj, 702);
        
        if (bob->txt_print == 100) {
            
            bob->txt_print = 0;
            
            object_add_text(obj, 0);
            text_t* txt = (text_t*) obj->txt->entry;
            text_add_string(txt, "Ich liebe dich auch!");
            obj->txt_print = 1;
            
            tsk->step++;
            
            return(true);
        }
    }
    
    if (tsk->step == 2) {
        
        if (obj->txt_print == 150) {
            
            obj->txt_print = 0;
            
            object_t* bob = object_get(obj, 702);
            
            waypoints_t* ways = bob->ways->entry;
            ways->active = true;
            bob->anim_walk = true;
            
            obj->can_move = true;
            obj->anim_walk = true;
            
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
