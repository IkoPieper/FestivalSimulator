#include "animation.h"

animation_t* animation_init(unsigned int id) {
	
	animation_t* anim = NULL;
	
	anim = (animation_t*) malloc(sizeof(animation_t));
	
	anim->id = id;
	anim->prev = NULL;
	anim->next = NULL;
	anim->cycle_first = NULL;
	anim->cycle = NULL;
	anim->delay_frames = 10;
	
	return(anim);
}

void animation_free(animation_t* anim) {
	
	surface_ring_t* cycle;
	surface_ring_t* cycle_next;
	
	cycle = anim->cycle_first;
	
	while (cycle != NULL) {
		cycle_next = cycle->next;
		SDL_FreeSurface(cycle->surf);
		free(cycle);
		cycle = cycle_next;
	}
	
	free(anim);
}


void animation_add_surface(animation_t* anim, SDL_Surface* surf) {
	
	surface_ring_t* cycle;
	cycle = (surface_ring_t*) malloc(sizeof(surface_ring_t));
	
	cycle->surf = surf;
	cycle->next = NULL;
	
	if (anim->cycle_first == NULL) {
		// add as first element:
		anim->cycle_first = cycle;
		anim->cycle = cycle;
	} else {
		// add as last element:
		surface_ring_t* cycle_tmp = anim->cycle_first;
		while (cycle_tmp->next != NULL) {
			cycle_tmp = cycle_tmp->next;
		}
		cycle_tmp->next = cycle;
	}
	
}

SDL_Surface* animation_get_next_surface(animation_t* anim, unsigned long frame) {
	
	if (frame % anim->delay_frames == 0) {
		if (anim->cycle->next == NULL) {
			anim->cycle = anim->cycle_first;
		} else {
			anim->cycle = anim->cycle->next;
		}
	}
	return(anim->cycle->surf);
}
