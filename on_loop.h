#ifndef _ON_LOOP_H_
    #define _ON_LOOP_H_

#include "sound.h"
#include "tasks.h"

void on_loop(
    groups_t* grp, sound_t* snd, verletbox_t* vbox, 
    bool* keys, uint64_t frame, float dt);
    
void on_loop_tasks(groups_t* grp, bool* keys, uint64_t frame, float dt);

void on_loop_items(object_t* hero, bool* keys, uint64_t frame);

void on_loop_animations(
    groups_t* grp, bool* keys, uint64_t frame, float dt);
    
void on_loop_sounds(
    groups_t* grp, bool* keys, sound_t* snd, uint64_t frame, float dt);

uint32_t on_loop_get_animation_walk(
    object_t* obj, float vel_x, float vel_y);

uint32_t on_loop_get_animation_walk_default(
    object_t* obj, float vel_x, float vel_y);

uint32_t on_loop_get_animation_walk_hand(
    object_t* obj, float vel_x, float vel_y);
    
uint32_t on_loop_get_animation_walk_pistol(
    object_t* obj, float vel_x, float vel_y);
    
void on_loop_waypoints(groups_t* grp, uint64_t frame, float dt);

#endif
