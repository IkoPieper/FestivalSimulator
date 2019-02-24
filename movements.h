#ifndef _MOVEMENTS_H_
    #define _MOVEMENTS_H_

#include "object.h"
#include <math.h>

void movements(object_t* obj, bool* keys);
void movements_hero(object_t* obj, bool* keys);
void movements_background(object_t* obj, object_t* obj_hero);
void movements_accelerate(object_t* obj);
void movements_screen_position(object_t* obj, object_t* obj_bg);
    
#endif
