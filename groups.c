#include "groups.h"

groups_t* groups_init(object_t* obj) {
    
    groups_t* grp = (groups_t*) malloc(sizeof(groups_t));
    
    grp->lst_securities = NULL;
    grp->lst_dixis = NULL;
    grp->lst_have_anim = NULL;
    grp->lst_have_ways = NULL;
    grp->lst_have_tsk = NULL;
    grp->lst_have_tsk_flunky_0 = NULL;
    grp->lst_have_tsk_flunky_1 = NULL;
    grp->obj_bg = NULL;
    grp->obj_hero = NULL;
    grp->obj_first = NULL;
    
    obj = object_get_first(obj);
    
    grp->obj_first = obj;
    
    while (obj != NULL) {
        
        if (obj->id == OBJECT_HERO_ID) {
            grp->obj_hero = obj;
        } else if (obj->id == OBJECT_BACKGROUND_ID) {
            grp->obj_bg = obj;
        }
        
        if (obj->is_security) {
            grp->lst_securities = create_before(
                grp->lst_securities, obj, obj->id);
        }
        
        /*if (obj->is_dixi) {
            grp->lst_dixis = create_before(
                grp->lst_dixis, obj, obj->id);
        }*/
        
        if (obj->anim != NULL) {
            grp->lst_have_anim = create_before(
                grp->lst_have_anim, obj, obj->id);
        }
        
        if (obj->ways != NULL) {
            grp->lst_have_ways = create_before(
                grp->lst_have_ways, obj, obj->id);
        }
        
        if (obj->tsk != NULL) {
            
            grp->lst_have_tsk = create_before(
                grp->lst_have_tsk, obj, obj->id);
            
            switch (obj->tsk->id) {
                case TASK_FLUNKY_0:
                    grp->lst_have_tsk_flunky_0 = create_before(
                        grp->lst_have_tsk_flunky_0, obj, obj->id);
                    break;
                case TASK_FLUNKY_1:
                    grp->lst_have_tsk_flunky_1 = create_before(
                        grp->lst_have_tsk_flunky_1, obj, obj->id);
                    break;
            }
        }
        
        obj = obj->next_object;
    }
    
    return(grp);
}

void groups_free(groups_t* grp) {
    
    delete_all(grp->lst_securities);
    delete_all(grp->lst_dixis);
    delete_all(grp->lst_have_anim);
    delete_all(grp->lst_have_ways);
    delete_all(grp->lst_have_tsk);
    
    free(grp);
}
