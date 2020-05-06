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
    
        if (tsk->step == 0 &&
            fabsf(obj->pos_x - bob->pos_x) < 150 &&
            fabsf(obj->pos_y - bob->pos_y) < 150) {
        
            object_add_text(bob, 0);
            text_t* txt = (text_t*) bob->txt->entry;
            text_add_string(txt, "Ich liebe dich!");
            bob->txt_print = 1;
            tsk->step++;
        }
    }
    
    if (tsk->step > 0) {
        
        object_t* bob = object_get(obj, 702);
        
        if (bob->txt_print == 0) {
            tsk->step = 0;
        }
    }
    
    return(false);
    
}

bool task_find_eva(task_t* tsk, object_t* obj, bool* keys, uint64_t frame) {
    
    return(false);
    
}
