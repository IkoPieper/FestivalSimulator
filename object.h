#ifndef _OBJECT_H_
    #define _OBJECT_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "list.h"
#include "meter.h"
#include "animation.h"
#include "text.h"
#include "waypoints.h"
#include "video.h"
#include "event.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>


typedef struct object object_t;
typedef struct task task_t;
typedef struct item item_t;
typedef struct collision collision_t;
typedef struct walls walls_t;

struct walls {
	// keep the surface pointer to free the pixels later:
	SDL_Surface* surf;
	// collision pixels:
	uint8_t* pxl;
	// x and y relative to main sprite position:
	uint32_t x;
	uint32_t y;
	// width and height:
	uint32_t w;
	uint32_t w_bmp;	// width in memory. see object_init_walls()
	uint32_t h;
	// position of most left pixel:
	uint32_t lx;
	uint32_t ly;
    uint32_t ly_beam; // use 1 below lowest left pixel for render beams
	// position of most right pixel:
	uint32_t rx;
	uint32_t ry;
    uint32_t ry_beam; // use 1 below lowest right pixel for render beams
	// line between those pixels:
	float slope;
	float offset;
};

struct collision {
    object_t* partner;
    bool is_in_blobb;
    float c_x;
    float c_y;
};

struct task {
    uint32_t step;
    void* variables;
    bool (*task_function)(task_t*, object_t*, bool*, uint64_t, float);
};

struct item {
    uint32_t id;
    SDL_Surface* surf;      // item picture
    void* variables;        // any additional variables
    uint32_t step;          // step in item use function
    bool (*item_function)(object_t*, object_t*, bool*, uint64_t);
};

struct object {
	
    // object id:
    uint32_t id;
    
	// object list:
	object_t* next_object;
	object_t* prev_object;
	
	// render lists:
    bool disable_render;
    list_t* render_before;      // this object needs to be rendered
                                // before the objects in this list
    list_t* render_after;       // this objects needs to be rendered
                                // after the objects in this list
    list_t* render_blobb;       // used to collect overlapping objects
    bool render_is_in_blobb;    // is the obj in this or another blobb?
    bool render_early;          // disable blobb stuff and render early
                                // TODO: could be removed
	// verlet boxes:
	object_t* next_vbox;
	object_t* prev_vbox;
	uint32_t vbox_x;
	uint32_t vbox_y;
	
	// physics:
	bool can_move;
	bool has_moved;
	float mass;
	float damping;
    float elasticity;
	float pos_x;        // position in relation to background
	float pos_y;
	float pos_x_old;    // position in previous frame
	float pos_y_old;
	float vel_x;
	float vel_y;
	bool vel_lock;		// if true: velocity is not allowed to change
	float acc_x;
	float acc_y;
	float acc_abs;
	
	// screen positions:
	float scr_pos_x;    // pos in relation to top left corner of screen
	float scr_pos_y;
	float max_scr_pos_x;
	float max_scr_pos_y;
	float min_scr_pos_x;
	float min_scr_pos_y;
	
	// bitmaps:
	SDL_Surface* surface;	// current picture
	//unsigned int* walls;	// aka collision zones
	walls_t* wall;	        // aka collision zones
	
    // tasks:
    list_t* tsk;		    // current task from list of tasks
    
    // item properties:
    item_t* itm_props;
    
    // items:
    list_t* itm;            // current item from list of items
    
    // meters:
    list_t* mtr;		    // displays important values
    
	// animations:
	list_t* anim;		    // current animation from list of animations
	bool anim_first_call;	// free surface if animation is called for 
							// the first time
                            
    bool anim_walk;         // use animations 1 to 8 for walk cycles
							
	// texts:
	char* txt_language;			// the language the object speaks
	list_t* txt;				// current text from list of texts
	SDL_Surface* txt_surface;	// string of current text as surface
	uint32_t txt_print;			// > 0 print current text
    float txt_next_letter;      // wait for next letter to print
	
	// waypoints:
	list_t* ways;
	
	// collisions:
	list_t* col;
    bool disable_collision;
    bool impulse_calculated;
};

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
void object_select_animation_target(object_t* obj, float x, float y);
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

collision_t* object_add_collision(object_t* obj, object_t* partner);
void object_remove_collision(object_t* obj, object_t* partner);
void object_free_collisions(list_t* col);

void object_add_task(object_t* obj, uint32_t id);
void object_free_tasks(list_t* lst);

void object_init_item_props(object_t* obj, SDL_Surface* surf, uint32_t id);
void object_free_item_props(item_t* itm_props);
void object_add_item(object_t* obj, object_t* obj_item, uint32_t id);
void object_free_items(object_t* obj);

// functions in object_tasks.c:
bool (*get_task_function(uint32_t id))(
    task_t* tsk, object_t* obj, bool* keys, uint64_t frame, float dt);
bool task_find_bob(
    task_t* tsk, object_t* obj, bool* keys, uint64_t frame, float dt);
bool task_find_eva(
    task_t* tsk, object_t* obj, bool* keys, uint64_t frame, float dt);
bool task_security_fence(
    task_t* tsk, object_t* obj, bool* keys, uint64_t frame, float dt);
bool task_bus_passenger(
    task_t* tsk, object_t* obj, bool* keys, uint64_t frame, float dt);
bool task_bus(
    task_t* tsk, object_t* obj, bool* keys, uint64_t frame, float dt);
void say(object_t* obj, uint32_t id, uint32_t duration);
bool said(object_t* obj);
void say_new(object_t* obj, char* str, uint32_t duration);
void say_free(object_t* obj);
void face(object_t* obj, object_t* obj_target, float dt);
void stop(object_t* obj);
void move_on(object_t* obj);
void drink_beer(object_t* obj, int16_t value);
void change_mood(object_t* obj, int16_t value);
void start_waypoints(object_t* obj, uint32_t id);
bool waypoints_finished(object_t* obj);

// functions in object_items.c:
bool (*get_item_function(uint32_t id))(object_t*, object_t*, bool*, uint64_t);
bool use_stone(object_t* obj, object_t* obj_partner, bool* keys, uint64_t frame);
bool use_red_stone(object_t* obj, object_t* obj_partner, bool* keys, uint64_t frame);
bool use_money(object_t* obj, object_t* obj_partner, bool* keys, uint64_t frame);

#define OBJECT_BACKGROUND_ID 1
#define OBJECT_HERO_ID 2
#define OBJECT_SCORE_ID 3
#define OBJECT_BUS 3001

#define TASK_FIND_BOB 0
#define TASK_FIND_EVA 1
#define TASK_SECURITY_FENCE 2
#define TASK_BUS_PASSENGER 3
#define TASK_BUS 4

#define ITEM_STONE 0
#define ITEM_RED_STONE 1
#define ITEM_MONEY 2

#endif
