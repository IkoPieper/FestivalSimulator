#ifndef _ANIMATION_H_
    #define _ANIMATION_H_

#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct surface_ring surface_ring_t;
typedef struct animation animation_t;

struct surface_ring {
	surface_ring_t* next;
	SDL_Surface* surf;
};

struct animation {
	surface_ring_t* cycle;
	surface_ring_t* cycle_first;
	uint32_t delay_frames;          // frames to wait for next surf
    float time_active;              // time animation is active
    bool surf_changed;               // true for one frame if surface 
                                    // has changed
    uint32_t n;                     // current surface number
};

animation_t* animation_init(float dt);
void animation_free(animation_t* anim);
void animation_add_surface(animation_t* anim, SDL_Surface* surf);
SDL_Surface* animation_get_next_surface(animation_t* anim, uint64_t frame);

#define ANIMATION_WALK_NORTH 1
#define ANIMATION_WALK_SOUTH 2
#define ANIMATION_WALK_WEST 3
#define ANIMATION_WALK_EAST 4
#define ANIMATION_REST_NORTH 5
#define ANIMATION_REST_SOUTH 6
#define ANIMATION_REST_WEST 7
#define ANIMATION_REST_EAST 8

#endif
