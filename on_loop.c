#include "on_loop.h"
#include "collisions.h"
#include "movements.h"
#include "animation.h"
#include "waypoints.h"

void on_loop(object_t* obj, verletbox_t* vbox, short* keys, unsigned long frame) {
	
	//Uint32 time;
	
	//time = SDL_GetTicks();
	collisions(obj, vbox);
	//printf("time for collisions: %d\n", SDL_GetTicks() - time);
	
	//time = SDL_GetTicks();
	movements(obj, keys);
	//printf("time for movements: %d\n", SDL_GetTicks() - time);
	
	//time = SDL_GetTicks();
	on_loop_animations(obj, keys, frame);
	
	on_loop_waypoints(obj, frame);
	//printf("time for animations and waypoints: %d\n", SDL_GetTicks() - time);

}

void on_loop_animations(object_t* obj, short* keys, unsigned long frame) {
	
	obj = object_get_first(obj);
	
	while (obj != NULL) {
		
		// select animation:
		if (obj->id == OBJECT_HERO_ID) {
			if        (keys[SDLK_w] && keys[SDLK_a]) {  // north west
				object_select_animation(obj, 5);
			} else if (keys[SDLK_w] && keys[SDLK_d]) {  // north east
				object_select_animation(obj, 6);
			} else if (keys[SDLK_s] && keys[SDLK_a]) {  // south west
				object_select_animation(obj, 7);
			} else if (keys[SDLK_s] && keys[SDLK_d]) {  // south east
				object_select_animation(obj, 8);
			} else if (keys[SDLK_w]) {  // north
				object_select_animation(obj, 1);
			} else if (keys[SDLK_s]) {  // south
				object_select_animation(obj, 2);
			} else if (keys[SDLK_a]) {  // west
				object_select_animation(obj, 3);
			} else if (keys[SDLK_d]) {  // east
				object_select_animation(obj, 4);
			} else {
				// stop animation
			}
		}
		
		// animate:
		if (obj->anim != NULL) {
			if (obj->id == OBJECT_HERO_ID) {
				if (keys[SDLK_w] || keys[SDLK_a] || keys[SDLK_s] || keys[SDLK_d]) {
					object_animate(obj, frame);
				}
			} else {
				object_animate(obj, frame);
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
