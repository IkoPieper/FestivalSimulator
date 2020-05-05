#ifndef _OBJECT_H_
    #define _OBJECT_H_

#include <SDL/SDL.h>
#include "list.h"
#include "animation.h"
#include "text.h"
#include "waypoints.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>


typedef struct object object_t;
typedef struct listobj listobj_t;
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
	// position of most right pixel:
	uint32_t rx;
	uint32_t ry;
	// line between those pixels:
	float slope;
	float offset;
};

struct collision {
	collision_t* next;
	collision_t* prev;
    object_t* partner;
    uint32_t area;
    uint32_t area_old;
    float c_x;
    float c_y;
    float c_x_old;
    float c_y_old;
    float vel_x;
    float vel_y;
};

struct listobj {
	listobj_t* next;
    object_t* obj;
};

struct object {
	
    // object id:
    uint32_t id;
    
	// object list:
	object_t* next_object;
	object_t* prev_object;
	
	// render lists:
    list_t* render_before;      // objects which need to be rendered
                                // before this object
    list_t* render_after;       // objects which need to be rendered
                                // after this object
    list_t* render_blobb;       // used to collect overlapping objects
    bool render_is_in_blobb;    // is the obj in this or another blobb?
    bool render_early;          // disable blobb stuff and render early
    
	GLuint render_id;           // openGL id
	
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
	
	// waypoints:
	list_t* ways;
	
	// collisions:
	list_t* col;
    bool disable_collision;
    
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

void object_add_animation(object_t* obj, uint32_t id);
void object_select_animation(object_t* obj, uint32_t id);
void object_animate(object_t* obj, uint64_t frame);
void object_free_animations(list_t* anim);

void object_add_text(object_t* obj, uint32_t id);
void object_select_text(object_t* obj, uint32_t id);
void object_print_text(object_t* obj);
void object_free_texts(list_t* txt);

void object_add_waypoints(object_t* obj, uint32_t id, uint32_t num_ways);
void object_select_waypoints(object_t* obj, uint32_t id);
void object_remove_waypoints(object_t* obj, uint32_t id);
void object_activate_waypoints(object_t* obj);
void object_get_next_waypoint(object_t* obj);
void object_aim_for_waypoint(object_t* obj);

collision_t* object_add_collision(object_t* obj, object_t* partner);
void object_remove_collision(object_t* obj, object_t* partner);



#ifndef OBJECT_SURFDISPLAY_ID
	#define OBJECT_SURFDISPLAY_ID 0
#endif

#ifndef OBJECT_BACKGROUND_ID
	#define OBJECT_BACKGROUND_ID 1
#endif

#ifndef OBJECT_HERO_ID
	#define OBJECT_HERO_ID 2
#endif

#ifndef OBJECT_SCORE_ID
	#define OBJECT_SCORE_ID 3
#endif

#endif
