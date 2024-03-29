/*
  Festival Simulator
  Copyright (C) 2018  Iko Pieper, Holger Buhl, Elli Pfeifer

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  Iko Pieper
  Uhlhornsweg 5
  26129 Oldenburg
  Germany
*/

#include "animation.h"

animation_t* animation_init(float dt) {
    
    animation_t* anim = NULL;
    
    anim = (animation_t*) malloc(sizeof(animation_t));
    
    anim->cycle_first = NULL;
    anim->cycle = NULL;
    anim->delay_frames = (uint32_t) (10.0 / dt);
    anim->delay_frames_wanted = anim->delay_frames;
    anim->time_active = 0.0;
    anim->surf_changed = false;
    anim->n = 0;
    
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

SDL_Surface* animation_get_next_surface(animation_t* anim, uint64_t frame) {
    
    if (frame % anim->delay_frames == 0) {
        
        if (anim->cycle->next == NULL) {
            anim->cycle = anim->cycle_first;
            anim->n = 0;
        } else {
            anim->cycle = anim->cycle->next;
            anim->n++;
        }
        anim->surf_changed = true;
        
    } else {
        
        anim->surf_changed = false;
    }
    return(anim->cycle->surf);
}
