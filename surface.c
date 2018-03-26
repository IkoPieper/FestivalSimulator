#include "surface.h"
 
SDL_Surface* surface_on_load(const char* File) {
	
	SDL_Surface* surf = SDL_LoadBMP(File);
 
	if (surf == NULL) {
		return(NULL);
	}
	
	// set the transparent pixel:
	SDL_SetColorKey(surf, SDL_SRCCOLORKEY, SDL_MapRGB(surf->format, 255, 0, 255));
 
	return(surf);
}

short surface_on_draw(SDL_Surface* surf_dest, SDL_Surface* surf_src, int x, int y) {
	if(surf_dest == NULL || surf_src == NULL) {
		return(0);
	}
 
	SDL_Rect rect_dest;
 
	rect_dest.x = x;
	rect_dest.y = y;
 
	SDL_BlitSurface(surf_src, NULL, surf_dest, &rect_dest);
 
	return(1);
}

