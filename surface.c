#include "surface.h"
 
SDL_Surface* surface_on_load(const char* File) {
    
    SDL_Surface* surf = SDL_LoadBMP(File);
 
    if (surf == NULL) {
        return(NULL);
    }
    
    // set the transparent pixel:
    SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 255, 0, 255));
 
    return(surf);
}

bool surface_on_draw(SDL_Surface* surf_dest, SDL_Surface* surf_src, int32_t x, int32_t y) {
    if(surf_dest == NULL || surf_src == NULL) {
        return(true);
    }
 
    SDL_Rect rect_dest;
 
    rect_dest.x = x;
    rect_dest.y = y;
 
    SDL_BlitSurface(surf_src, NULL, surf_dest, &rect_dest);
 
    return(false);
}

// bitmaps are stored as 32 bit blocks in memory. as we use
// 8 bit per pixel, we have to account for additional junk
// pixels that might be stored at the end of every row:
uint32_t surface_get_bmp_width(SDL_Surface* surf) {
    
    if (surf->w % 4 == 0) {
        return(surf->w);
    }
    
    return(surf->w + (4 - (surf->w % 4)));
}
