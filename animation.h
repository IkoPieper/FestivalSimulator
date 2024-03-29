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
    uint32_t delay_frames_wanted;   // the original value from config
    float time_active;              // time animation is active
    bool surf_changed;              // true for one frame if surface 
                                    // has changed
    uint32_t n;                     // current surface number
};

animation_t* animation_init(float dt);

void animation_free(animation_t* anim);

void animation_add_surface(animation_t* anim, SDL_Surface* surf);

SDL_Surface* animation_get_next_surface(
    animation_t* anim, uint64_t frame);

#endif
