#ifndef _OBJECT_H_
    #define _OBJECT_H_

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "animation.h"
#include <GL/gl.h>
#include <GL/glu.h>

typedef struct object object_t;
typedef struct collision collision_t;

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
	
	// animations:
	animation_t* anim;
	animation_t* anim_next;
	animation_t* anim_prev;
	
	// properties:
	float pos_x;
	float pos_y;
	float pos_x_old;
	float pos_y_old;
	float vel_x;
	float vel_y;
	short vel_lock;		// if true: velocity is not allowed to change
	float acc_x;
	float acc_y;
	float acc_abs;
	float damping;
	float mass;
	int scr_pos_x;
	int scr_pos_y;
	int scr_pos_x_old;
	int scr_pos_y_old;
	int max_scr_pos_x;
	int max_scr_pos_y;
	int min_scr_pos_x;
	int min_scr_pos_y;
	SDL_Surface* surface;	// current picture
	unsigned int* walls;	// aka collision zones
	TTF_Font* font;
	
	// collisions:
	collision_t* col;
    short disable_collision;
    short collision_stop;
    
};

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
