#include "items.h"

void items_init_object_item_props(
    object_t* obj, SDL_Surface* surf, uint32_t id) {
    
    obj->itm_props = (item_t*) malloc(sizeof(item_t));
    obj->itm_props->id = id;
    obj->itm_props->render_after_host = false;
    obj->itm_props->surf = surf;
    obj->itm_props->variables = NULL;
    obj->itm_props->step = 0;
    obj->itm_props->item_function = get_item_function(id);
}

void items_free_object_item_props(object_t* obj) {
    
    item_t* itm_props = obj->itm_props;
    
    if (itm_props != NULL) {
        
        if (itm_props->surf != NULL) {
            SDL_FreeSurface(itm_props->surf);
        }
        
        free(itm_props);
        
        obj->itm_props = NULL;
    }
}

void items_add_to_object(object_t* obj, object_t* obj_item, uint32_t id) {
    
    obj_item->disable_collision = true;
    obj_item->disable_render = true;
    obj->itm = create_before(obj->itm, obj_item, id);
}

void items_free(object_t* obj) {

    list_t* lst = obj->itm;

    if (lst != NULL) {
        
        delete_all(lst);
        obj->itm = NULL;
    }
}

bool (*get_item_function(uint32_t id))(object_t*, object_t*, bool*, uint64_t) {
    
    switch (id) {
        case ITEM_MONEY: return(&use_money); break;
        case ITEM_WATER_PISTOL: return(&use_water_pistol); break;
        case ITEM_HAND: return(&use_hand); break;
    }
    
    return(NULL);
}

bool use_money(
    object_t* obj, object_t* obj_host, bool* keys, uint64_t frame) {
    
    return(false);
}

bool use_water_pistol(
    object_t* obj, object_t* obj_host, bool* keys, uint64_t frame) {
    
    if (keys[KEY_SPACE] && obj->itm_props->step == 0) {
        
        obj->can_move = true;
        
        obj->itm_props->step = 1;
        
        return(true);
    }
    
    if (obj->itm_props->step == 1) {
        
        if (keys[KEY_SPACE]) {
            
            // render item after obj_host:
            obj->itm_props->render_after_host = true;
            
            switch (obj_host->facing) {
                
                case OBJECT_FACING_NORTH:
                    if (obj->anim->id != 1) {
                        object_select_animation(obj, 1);
                    }
                    obj->pos_x = obj_host->pos_x + 15;
                    obj->pos_y = obj_host->pos_y - 20;
                    obj->wall->x = -17;
                    obj->wall->y = 50;
                    break;
                    
                case OBJECT_FACING_SOUTH:
                    if (obj->anim->id != 2) {
                        object_select_animation(obj, 2);
                    }
                    obj->pos_x = obj_host->pos_x + 7;
                    obj->pos_y = obj_host->pos_y + 38;
                    obj->wall->x = -17;
                    obj->wall->y = 10;
                    break;
                    
                case OBJECT_FACING_WEST:
                    if (obj->anim->id != 3) {
                        object_select_animation(obj, 3);
                    }
                    obj->pos_x = obj_host->pos_x - 30;
                    obj->pos_y = obj_host->pos_y + 33;
                    obj->wall->x = 20;
                    obj->wall->y = 10;
                    break;
                    
                case OBJECT_FACING_EAST:
                    if (obj->anim->id != 4) {
                        object_select_animation(obj, 4);
                    }
                    obj->pos_x = obj_host->pos_x + 17;
                    obj->pos_y = obj_host->pos_y + 33;
                    obj->wall->x = 0;
                    obj->wall->y = 10;
                    break;
            }
            
        } else {
            
            obj->itm_props->render_after_host = false;
            
            obj->can_move = false;
            
            obj->itm_props->step = 0;
        }
        
        return(true);
    }
    
    return(false);
}

bool use_hand(
    object_t* obj, object_t* obj_host, bool* keys, uint64_t frame) {
    
    // pick up target object using space bar:
    if (obj->itm_props->step == 0 && keys[KEY_SPACE] && obj_host->col != NULL) {
        
        printf("use_hand\n");
            
        // get collision partner:
        collision_t* col = (collision_t*) obj_host->col->entry;
        object_t* obj_target = col->partner;
        
        printf("use_hand: obj_partner->id: %d\n", obj_target->id);
        
        // start carring partner around:
        pick_up(obj_host, obj_target);
        obj->itm_props->step = 1;
        
        return(true);
    }
    
    // wait for space bar release:
    if (obj->itm_props->step == 1 && !keys[KEY_SPACE]) {
        obj->itm_props->step = 2;
    }
    
    // throw target object on second space bar press:
    if (obj->itm_props->step == 2 && keys[KEY_SPACE]) {
        
        throw(obj_host);
        obj->itm_props->step = 3;
    }
    
    // wait for space bar release:
    if (obj->itm_props->step == 3 && !keys[KEY_SPACE]) {
        obj->itm_props->step = 0;
    }
    
    return(false);
}
