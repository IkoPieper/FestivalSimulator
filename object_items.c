#include "object.h"

bool (*get_item_function(uint32_t id))(object_t*, object_t*, bool*, uint64_t) {
    
    switch (id) {
        case ITEM_STONE: return(&use_stone); break;
        case ITEM_RED_STONE: return(&use_red_stone); break;
        case ITEM_MONEY: return(&use_money); break;
    }
    
    return(NULL);
}

bool use_stone(
    object_t* obj, object_t* obj_partner, bool* keys, uint64_t frame) {
    
    if (obj->itm_props->step == 0) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
        say_new(hero, "Schau mal, ich habe einen Stein!", 150);
        
        obj->itm_props->step++;
        
        return(1);
    }
    
    if (obj->itm_props->step == 1) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
        
        if (said(hero)) {
            say_free(hero);
            obj->itm_props->step = 0;
            
            return(1);
        }
    }
    
    return(0);
}

bool use_red_stone(
    object_t* obj, object_t* obj_partner, bool* keys, uint64_t frame) {
    
    static bool stone_in_use = false;
    
    if (obj->itm_props->step == 0) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
        say_new(hero, "Schau mal, ich habe einen roten Stein!", 150);
        
        stone_in_use = true;
        obj->itm_props->variables = &stone_in_use;
        
        obj->itm_props->step++;
        
        return(1);
    }
    
    if (obj->itm_props->step == 1) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
        
        if (said(hero)) {
            say_free(hero);
            obj->itm_props->step = 0;
            
            stone_in_use = false;
            
            return(1);
        }
    }
    
    return(0);
}

bool use_money(
    object_t* obj, object_t* obj_partner, bool* keys, uint64_t frame) {
    
    return(0);
}
