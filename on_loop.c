#include "on_loop.h"
#include "collisions.h"
#include "movements.h"
#include "animation.h"
#include "waypoints.h"

void on_loop(object_t* obj, verletbox_t* vbox, short* keys, unsigned long frame) {
	
	Uint32 time;
	
	time = SDL_GetTicks();
	collisions(obj, vbox);
	printf("time for collisions: %d\n", SDL_GetTicks() - time);
	
	movements(obj, keys);
	
	on_loop_animations(obj, keys, frame);
	
	on_loop_waypoints(obj, frame);
	
}

void on_loop_animations(object_t* obj, short* keys, unsigned long frame) {
	
	obj = object_get_first(obj);
	
	while (obj != NULL) {
		
		// select animation:
		if (obj->id == OBJECT_HERO_ID) {
			if (keys[SDLK_w] || keys[SDLK_d]) {
				object_select_animation(obj, 1);
			} else if (keys[SDLK_s] || keys[SDLK_a]) {
				object_select_animation(obj, 2);
			} else {
				// stop animation
			}
		}
		
		// animate:
		if (obj->anim != NULL) {
			if (obj->id == OBJECT_HERO_ID) {
				if (keys[SDLK_w] || keys[SDLK_a] || keys[SDLK_s] || keys[SDLK_d]) {
					obj->surface = animation_get_next_surface(obj->anim, frame);
				}
			} else {
				obj->surface = animation_get_next_surface(obj->anim, frame);
			}
		}
		obj = obj->next_object;
	}
	
}

void on_loop_waypoints(object_t* obj, unsigned long frame) {
	
	obj = object_get_first(obj);
	
	while (obj != NULL) {
			
		if (obj->ways != NULL && obj->ways->active && !obj->vel_lock) {

			object_get_next_waypoint(obj);
			object_aim_for_waypoint(obj);
			
		}
	
		obj = obj->next_object;
	}
	
}
