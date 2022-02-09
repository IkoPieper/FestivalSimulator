#ifndef _DATATYPES_H_
    #define _DATATYPES_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdint.h>
#include "list.h"

typedef struct task task_t;
typedef struct walls walls_t;
typedef struct item item_t;
typedef struct collision collision_t;
typedef struct object object_t;
typedef struct groups groups_t;

typedef void (*task_function_t) (
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt);

struct task {
    uint32_t id;
    uint32_t step;          // step in task function
    uint32_t counter;       // usefull counter
    void* variables;        // any additional variables
    void* variables_shared; // variables shared with other objects which have
                            // the same task
    task_function_t task_function;      // called every frame for the object
                                        // that owns the task
    task_function_t task_function_free; // free variables, called when needed
};

struct walls {
	// keep the surface pointer to free the pixels later:
	SDL_Surface* surf;
	// collision pixels:
	uint8_t* pxl;
	// x and y relative to main sprite position:
	int32_t x;
	int32_t y;
    // x and y shifts relative to original x and y position:
	int32_t x_shift;
	int32_t y_shift;
	// width and height:
	uint32_t w;
	uint32_t w_bmp;	// width in memory. see object_init_walls()
	uint32_t h;
	// position of most left pixel:
	int32_t lx;
	int32_t ly;
    int32_t ly_beam; // use 1 below lowest left pixel for render beams
	// position of most right pixel:
	int32_t rx;
	int32_t ry;
    int32_t ry_beam; // use 1 below lowest right pixel for render beams
	// line between those pixels:
	float slope;
	float offset;
};

struct item {
    uint32_t id;
    SDL_Surface* surf;      // item picture
    uint32_t step;          // step in item use function
    bool render_after_host; // render directly after the host object
    void* variables;        // any additional variables
    bool (*item_function)(object_t*, object_t*, bool*, uint64_t, float);
};

struct collision {
    object_t* partner;
    float c_x;
    float c_y;
    bool use_for_impulse;
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
    bool in_vbox;
	
	// physics:
	bool can_move;
	bool has_moved;
	float mass;
	float damping;
    bool disable_damping;
    float elasticity;
	float pos_x;        // position in relation to background
	float pos_y;
	float pos_x_old;    // position in previous frame
	float pos_y_old;
	float vel_x;
	float vel_y;
	float acc_x;
	float acc_y;
	float acc_abs;
    
    // object can be carried by another object:
    bool can_be_carried;
    
    // carried by another object:
    object_t* obj_carried_by;
    
    // carries another object:
    object_t* obj_carries;
	
    // disable collision until object escaped collision zone (used for throws):
    object_t* obj_escape_col;
    float obj_escape_col_time;
    
    // facing direction:
    uint8_t facing;
    
	// screen positions:
	float scr_pos_x;    // pos in relation to top left corner of screen
	float scr_pos_y;
    
	// bitmaps:
	SDL_Surface* surface;	// current picture
	
    // walls (aka collision zones):
	walls_t* wall;
	
    // tasks:
    bool is_security;
    bool is_hunted_by_security;
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
    bool anim_carry;        // use animations 9 to 16 for carry walk cycles
    bool anim_pistol;       // use animations 17 to 24 for pistol walk cycles
							
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
    uint32_t col_sample_timer;
    bool disable_collision;
};

struct groups {
	list_t* lst_securities; // objects that are securities
	list_t* lst_dixis;      // objects that are dixis
    list_t* lst_have_anim;  // objects that have animations
    list_t* lst_have_ways;  // objects that have waypoints
    list_t* lst_have_tsk;   // objects that have tasks
    list_t* lst_have_tsk_flunky_0;
    list_t* lst_have_tsk_flunky_1;
    object_t* obj_bg;       // the object that acts as background
    object_t* obj_hero;     // the hero of the game
    object_t* obj_first;    // first object in object list
};

#endif
