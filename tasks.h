#ifndef _TASKS_H_
    #define _TASKS_H_

#include "object.h"
#include "collisions.h"
#include "groups.h"
#include "list.h"
#include "meter.h"
#include "animation.h"
#include "text.h"
#include "waypoints.h"
#include "tasks.h"
#include "items.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>


typedef struct task task_t;

struct task {
    uint32_t step;          // step in task function
    uint32_t counter;       // usefull counter
    void* variables;        // any additional variables
    bool (*task_function)(
        task_t* tsk, object_t* obj, groups_t* grp, 
        bool* keys, uint64_t frame, float dt);
};

void tasks_add_to_object(object_t* obj, uint32_t id);
void tasks_free(groups_t* grp);

bool (*get_task_function(uint32_t id))(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
bool task_find_bob(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
bool task_find_eva(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
bool task_security_fence(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
bool task_bus_passenger(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
bool task_bus(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
    
typedef struct hunt hunt_t;
struct hunt {
    object_t* obj_hunted;
    bool clockwise;
};

bool task_hunt(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);

void hunt_object(object_t* obj, task_t* tsk, bool clockwise, 
    object_t* obj_hunted, float dt);

#define TASK_FIND_BOB 0
#define TASK_FIND_EVA 1
#define TASK_SECURITY_FENCE 2
#define TASK_BUS_PASSENGER 3
#define TASK_BUS 4
#define TASK_HUNT 5

#endif
