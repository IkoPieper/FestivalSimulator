#ifndef _ON_LOOP_H_
    #define _ON_LOOP_H_

#include <SDL/SDL.h>
#include <math.h>
#include "object.h"
#include "verletbox.h"
#include "event.h"

void on_loop(object_t* obj, verletbox_t* vbox, bool* keys, uint64_t frame);
void on_loop_tasks(object_t* obj, bool* keys, uint64_t frame);
void on_loop_animations(object_t* obj, bool* keys, uint64_t frame);
uint32_t on_loop_get_animation_walk_hero(uint32_t anim_id, bool* keys);
uint32_t on_loop_get_animation_walk(uint32_t anim_id, float vel_x, float vel_y);
void on_loop_waypoints(object_t* obj, uint64_t frame);

#endif
