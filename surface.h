#ifndef _SURFACE_H_
    #define _SURFACE_H_
 
#include <SDL/SDL.h>
 
SDL_Surface* surface_on_load(const char* File);
short surface_on_draw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y);

#endif
