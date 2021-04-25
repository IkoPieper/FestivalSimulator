#ifndef _ITEMS_H_
    #define _ITEMS_H_

#include "object.h"

void items_init_object_item_props(
    object_t* obj, SDL_Surface* surf, uint32_t id);
    
void items_free_object_item_props(object_t* obj);

void items_add_to_object(
    object_t* obj, object_t* obj_item, uint32_t id);
    
void items_free(object_t* obj);

bool (*get_item_function(uint32_t id))(
    object_t*, object_t*, bool*, uint64_t, float);
    
bool use_money(
    object_t* obj, object_t* obj_host, bool* keys, uint64_t frame, float dt);
    
bool use_water_pistol(
    object_t* obj, object_t* obj_host, bool* keys, uint64_t frame, float dt);
    
bool use_hand(
    object_t* obj, object_t* obj_host, bool* keys, uint64_t frame, float dt);

#define ITEM_MONEY 0
#define ITEM_WATER_PISTOL 1
#define ITEM_HAND 2

#endif
