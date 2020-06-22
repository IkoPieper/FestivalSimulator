#ifndef _ITEMS_H_
    #define _ITEMS_H_

#include "object.h"
#include "groups.h"
#include "list.h"
#include "meter.h"
#include "animation.h"
#include "text.h"
#include "waypoints.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

void items_init_object_item_props(
    object_t* obj, SDL_Surface* surf, uint32_t id);
void items_free_object_item_props(object_t* obj);
void items_add_to_object(
    object_t* obj, object_t* obj_item, uint32_t id);
void items_free(object_t* obj);

bool (*get_item_function(uint32_t id))(object_t*, object_t*, bool*, uint64_t);
bool use_stone(object_t* obj, object_t* obj_partner, bool* keys, uint64_t frame);
bool use_red_stone(object_t* obj, object_t* obj_partner, bool* keys, uint64_t frame);
bool use_money(object_t* obj, object_t* obj_partner, bool* keys, uint64_t frame);
bool use_water_pistol(object_t* obj, object_t* obj_partner, bool* keys, uint64_t frame);

#define ITEM_STONE 0
#define ITEM_RED_STONE 1
#define ITEM_MONEY 2
#define ITEM_WATER_PISTOL 3

#endif
