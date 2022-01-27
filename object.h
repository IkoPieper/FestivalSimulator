#ifndef _OBJECT_H_
    #define _OBJECT_H_

#include "datatypes.h"
#include "constants.h"
#include "meter.h"
#include "animation.h"
#include "text.h"
#include "waypoints.h"
#include "video.h"
#include <math.h>

walls_t* object_init_walls(SDL_Surface* surf_wall, SDL_Surface* surf);
void object_free_walls(walls_t* wall);

object_t* object_get_last(object_t* obj);
object_t* object_get_first(object_t* obj);
object_t* object_add(object_t* obj, uint32_t id);
object_t* object_remove(object_t* obj, uint32_t id);
void object_clean_up(object_t* obj);
object_t* object_get(object_t* obj, uint32_t id);
uint32_t object_get_count(object_t* obj);

void object_add_meter(object_t* obj, uint32_t id, 
    uint8_t type, float scr_pos_x, float scr_pos_y);
void object_free_meters(list_t* mtr);

void object_add_animation(object_t* obj, uint32_t id, float dt);
void object_select_animation(object_t* obj, uint32_t id);
void object_animate(object_t* obj, uint64_t frame, float dt);
void object_free_animations(list_t* anim);

void object_add_text(object_t* obj, uint32_t id);
void object_select_text(object_t* obj, uint32_t id);
void object_print_text(object_t* obj);
void object_remove_text(object_t* obj, uint32_t id);
void object_free_texts(list_t* txt);

void object_add_waypoints(object_t* obj, uint32_t id, uint32_t num_ways);
void object_select_waypoints(object_t* obj, uint32_t id);
void object_activate_waypoints(object_t* obj);
void object_get_next_waypoint(object_t* obj, float dt);
void object_aim_for_waypoint(object_t* obj);
void object_free_waypoints(list_t* ways);

// functions shared by tasks and items:
void say(object_t* obj, uint32_t id, uint32_t duration);
bool said(object_t* obj);
void say_new(object_t* obj, char* str, uint32_t duration);
void say_free(object_t* obj);
bool move_to_position(object_t* obj, float x, float y, float vel_abs);
bool move_to_relative(object_t* obj, float x, float y, float vel_abs);
void hunt_object(object_t* obj, uint32_t* counter, bool clockwise, 
    object_t* obj_hunted, float dt);
void face(object_t* obj, object_t* obj_target, float dt);
void stop(object_t* obj);
void move_on(object_t* obj);
meter_t* meter_get(object_t* obj, uint8_t type);
void drink_beer(object_t* obj, int16_t value);
void change_mood(object_t* obj, int16_t value);
bool check_collision(object_t* obj1, uint32_t id_obj2);
bool pick_up(object_t* obj, object_t* obj_target);
void put_down(object_t* obj);
void throw(object_t* obj, float vel_add);
void start_waypoints(object_t* obj, uint32_t id);
bool waypoints_finished(object_t* obj);
float squared_distance(object_t* obj1, object_t* obj2);
bool squared_distance_smaller(
    object_t* obj1, object_t* obj2, float dist_squared);
bool squared_distance_greater(
    object_t* obj1, object_t* obj2, float dist_squared);

#endif
