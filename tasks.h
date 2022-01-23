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

typedef struct team_game team_game_t;
struct team_game {
    bool team_a;            // if true: obj is in team a
    bool team_b;            // if true: obj is in team b
};

typedef struct team_game_shared team_game_shared_t;
struct team_game_shared {
    object_t** team_a;          // array of team a players TODO: needed?
    object_t** team_b;          // array of team b players TODO: needed?
    uint8_t num_finished;       // number of players that finished the round
    bool team_a_turn;           // team a's turn to throw the ball
    bool team_b_turn;           // team b's turn to throw the ball
    uint8_t num_player_team_a;
    uint8_t num_player_team_b;
};

bool tasks_is_players_turn(team_game_t* t, team_game_shared_t* t_shared);
bool tasks_team_has_finished(team_game_t* t, team_game_shared_t* t_shared);
void tasks_switch_teams(team_game_shared_t* t_shared);
void tasks_team_set_step(
    uint32_t step, object_t** team, uint8_t num_player, uint32_t id);

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

void hunt_object(object_t* obj, uint32_t* counter, bool clockwise, 
    object_t* obj_hunted, float dt);

typedef struct flunky_shared flunky_shared_t;
struct flunky_shared {
    team_game_shared_t* teams;
    uint32_t pos_y_line_team_a;
    uint32_t pos_y_line_team_b;
    object_t* ball;
    object_t* target;
    float pos_x_target;         // default position of target
    float pos_y_target;
    bool target_hit;            // target bottle got hit
    bool target_retrieved;
    bool ball_retrieved;
};

typedef struct flunky flunky_t;
struct flunky {
    flunky_shared_t* shared;// task variables shared between objects
    team_game_t* teams;
    bool is_player;         // if true: object is human player
    bool is_ball;           // if true: object is the ball
    bool is_target;         // if true: object is the target bottle
    bool is_beer;           // if true: object is a beer bottle
    object_t* beer;         // every player has a beer
    float pos_x_default;    // default x-position
    float pos_y_default;    // default y-position
    float pos_x_next;       // currently targeted x-position
    float pos_y_next;       // currently targeted y-position
    uint32_t counter;       // used in hunt_object() for running after target and ball
    bool clockwise;         // used in hunt_object() for running after target and ball
};

void task_flunky_init(list_t* lst_obj, uint32_t id);

void task_flunky_free_shared(void* var_shared, uint32_t id);

void task_flunky(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);
    
void task_flunky_free(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);

// TODO: remove unused parameters in flunky functions below

void task_flunky_player(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt);

void task_flunky_beer(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt);
    
void task_flunky_target(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt);

void task_flunky_ball(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt);
    
void task_flunky_player_drink(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt);

void task_flunky_player_throw_ball(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt);

void task_flunky_player_retrieve_ball_or_target(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt, bool ball_only);

// final step is used to set local parameters back to init values:
#define TASK_FINAL_STEP 666

#endif
