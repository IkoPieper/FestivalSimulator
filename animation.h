#ifndef _ANIMATION_H_
    #define _ANIMATION_H_

#include <SDL/SDL.h>

typedef struct surface_ring surface_ring_t;
typedef struct animation animation_t;

struct surface_ring {
	surface_ring_t* next;
	SDL_Surface* surf;
};

struct animation {
	uint32_t id;
	animation_t* prev;
	animation_t* next;
	surface_ring_t* cycle;
	surface_ring_t* cycle_first;
	uint32_t delay_frames;
};

animation_t* animation_init(uint32_t id);
void animation_free(animation_t* anim);
void animation_add_surface(animation_t* anim, SDL_Surface* surf);
SDL_Surface* animation_get_next_surface(animation_t* anim, uint64_t frame);

#endif
