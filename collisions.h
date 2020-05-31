#ifndef _COLLISION_H_
    #define _COLLISION_H_

//#include "datatypes.h"
#include "object.h"
#include "verletbox.h"
#include <math.h>
#include <stdbool.h>

void collisions(object_t* obj, verletbox_t* vbox, float dt);
bool collisions_check(object_t* obj1, object_t* obj2, float dt);
void collisions_impulse(
	object_t* obj1, object_t* obj2, 
	collision_t* col1, collision_t* col2);
void collisions_update_render(object_t* obj, object_t* obj_b);
bool collisions_beam(int32_t x, int32_t y, object_t* obj);
    
#endif
