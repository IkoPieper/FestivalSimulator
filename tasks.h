#ifndef _TASKS_H_
    #define _TASKS_H_

#include "object.h"
#include "items.h"
#include "collisions.h"

void tasks_init(groups_t* grp);

void tasks_add_to_object(object_t* obj, uint32_t id);
void tasks_free(groups_t* grp);

void tasks_get_functions(task_t* tsk, uint32_t id);

task_t* tasks_get_task(object_t* obj, uint32_t id);

void tasks_share_variables(list_t* lst_obj, void* var_shared, uint32_t id);

void task_find_bob(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
    
void task_find_eva(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
    
void task_security_fence(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
    
void task_bus_passenger(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
    
void task_bus(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);

void task_soccer(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
    
void task_soccer_free(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
    
void task_soccer_ball(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
    
typedef struct hunt hunt_t;
struct hunt {
    object_t* obj_hunted;
    bool clockwise;
};

void task_hunt(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
    
void task_hunt_free(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);

void hunt_object(object_t* obj, task_t* tsk, bool clockwise, 
    object_t* obj_hunted, float dt);

typedef struct flunky_shared flunky_shared_t;
struct flunky_shared {
    uint32_t pos_y_line_team_a;
    uint32_t pos_y_line_team_b;
    object_t* ball;
    object_t* bottle;
    object_t** team_a;
    object_t** team_b;
    uint8_t num_player_team_a;
    uint8_t num_player_team_b;
};
typedef struct flunky flunky_t;
struct flunky {
    flunky_shared_t* shared;// task variables shared between objects
    bool is_player;         // if true: object is human player
    bool is_ball;           // if true: object is the ball
    bool is_bottle;         // if true: object is the bottle
    bool team_a;            // if true: obj is in team a
    bool team_b;            // if true: obj is in team b
    uint32_t pos_x_line;    // start x-position behind line
    uint32_t pos_y_line;    // start y-position behind line
};

void task_flunky_init(list_t* lst_obj, uint32_t id);

void task_flunky_free_shared(void* var_shared, uint32_t id);

void task_flunky(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
    
void task_flunky_free(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);

#endif
