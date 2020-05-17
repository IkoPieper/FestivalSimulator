#ifndef _SURFACE_H_
    #define _SURFACE_H_
 
#include <SDL2/SDL.h>
#include <stdbool.h>
 
SDL_Surface* surface_on_load(const char* File);
bool surface_on_draw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int32_t X, int32_t Y);

#endif
