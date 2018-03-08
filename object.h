#ifndef _OBJECT_H_
    #define _OBJECT_H_

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "animation.h"
#include "waypoints.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

typedef struct object object_t;
typedef struct collision collision_t;
typedef struct walls walls_t;

struct walls {
	// collision pixels:
	unsigned int* pxl;
	// x and y relative to main sprite position:
	unsigned int x;
	unsigned int y;
	// width and height:
	unsigned int w;
	unsigned int h;
	// position of most left pixel:
	unsigned int lx;
	unsigned int ly;
	// position of most right pixel:
	unsigned int rx;
	unsigned int ry;
	// line between those pixels:
	float slope;
	float offset;
};

struct collision {
	collision_t* next;
	collision_t* prev;
    object_t* partner;
    unsigned int area;
    unsigned int area_old;
    float c_x;
    float c_y;
    float c_x_old;
    float c_y_old;
    float vel_x;
    float vel_y;
};

struct object {
	// list variables:
	object_t* next_object;
	object_t* prev_object;
	unsigned int id;
	
	// render:
	object_t* next_render;
	object_t* prev_render;
	GLuint render_id;
	
	// verlet boxes:
	object_t* next_vbox;
	object_t* prev_vbox;
	unsigned int vbox_x;
	unsigned int vbox_y;
	
	// animations:
	animation_t* anim;

	// waypoints:
	waypoints_t* ways;
	
	// properties:
	short can_move;
	float pos_x;        // position in relation to background
	float pos_y;
	float pos_x_old;    // position in previous frame
	float pos_y_old;
	float vel_x;
	float vel_y;
	short vel_lock;		// if true: velocity is not allowed to change
	float acc_x;
	float acc_y;
	float acc_abs;
	float damping;
	float mass;
	float scr_pos_x;      // pos in relation to top left corner of screen
	float scr_pos_y;
	float max_scr_pos_x;
	float max_scr_pos_y;
	float min_scr_pos_x;
	float min_scr_pos_y;
	
	
	SDL_Surface* surface;	// current picture
	//unsigned int* walls;	// aka collision zones
	walls_t* wall;	// aka collision zones
	TTF_Font* font;
	
	// collisions:
	collision_t* col;
    short disable_collision;
    
};

walls_t* object_init_walls(SDL_Surface* surf_wall, SDL_Surface* surf);

object_t* object_get_last(object_t* obj);
object_t* object_get_first(object_t* obj);
object_t* object_get_first_render(object_t* obj);
object_t* object_add(object_t* obj, unsigned int id);
object_t* object_remove(object_t* obj, unsigned int id);
void object_clear(object_t* obj);
object_t* object_get(object_t* obj, unsigned int id);
unsigned int object_get_count(object_t* obj);

void object_add_animation(object_t* obj, unsigned int id);
void object_select_animation(object_t* obj, unsigned int id);
void object_remove_animation(object_t* obj, unsigned int id);
void object_remove_selected_animation(object_t* obj);

void object_add_waypoints(object_t* obj, unsigned int id, unsigned int num_ways);
void object_select_waypoints(object_t* obj, unsigned int id);
void object_remove_waypoints(object_t* obj, unsigned int id);
void object_remove_selected_waypoints(object_t* obj);
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
