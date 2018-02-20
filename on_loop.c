#include "on_loop.h"
#include "collisions.h"
#include "movements.h"
#include "animation.h"

void on_loop(object_t* obj, short* keys, unsigned long frame) {
	
	collisions(obj);
	movements(obj, keys);
	on_loop_animations(obj, keys, frame);
	
}

void on_loop_animations(object_t* obj, short* keys, unsigned long frame) {
	
	obj = object_get_first(obj);
	
	while (obj->next_object != NULL) {
		
		// select animation:
		if (obj->id == OBJECT_HERO_ID) {
			if (keys[SDLK_d]) {
				object_select_animation(obj, 2);
			} else if (keys[SDLK_w] || keys[SDLK_a] || keys[SDLK_s]) {
				object_select_animation(obj, 1);
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
