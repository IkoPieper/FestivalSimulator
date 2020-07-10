#ifndef _MOVEMENTS_H_
    #define _MOVEMENTS_H_

#include "object.h"
#include "groups.h"
#include "event.h"
#include <math.h>

void movements(groups_t* grp, bool* keys, float dt);
void movements_hero(object_t* obj, bool* keys);
void movements_background(object_t* obj, object_t* obj_hero);
void movements_carried(object_t* obj, object_t* obj_host);
void movements_accelerate(object_t* obj, float dt);
void movements_screen_position(object_t* obj, object_t* obj_bg);
    
#endif
