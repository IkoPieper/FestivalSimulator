#include "surface.h"
 
SDL_Surface* surface_on_load(const char* File) {
	
	SDL_Surface* surf = NULL;
	SDL_Surface* surf_tmp = NULL;
 
	if((surf_tmp = SDL_LoadBMP(File)) == NULL) {
		return NULL;
	}
 
	SDL_SetColorKey(surf_tmp, SDL_SRCCOLORKEY, SDL_MapRGB(surf_tmp->format, 255, 0, 255));
	// function allocates memory for a new surface in display format
	surf = SDL_DisplayFormat(surf_tmp);	
	SDL_FreeSurface(surf_tmp);
 
	/*fprintf(stderr, "Rmask: %x\n", surf->format->Rmask);
	fprintf(stderr, "Gmask: %x\n", surf->format->Gmask);
	fprintf(stderr, "Bmask: %x\n", surf->format->Bmask);
	fprintf(stderr, "Amask: %x\n", surf->format->Amask);*/
 
	if (surf->format->Rmask == 0x00ff0000) {
		// switch color channels:
		unsigned int* pxl = (unsigned int*) surf->pixels;
		Uint8* pxl8;
		Uint8 pxl_tmp;
		int n;
		for (n = 0; n < surf->w * surf->h; n++) {
			pxl8 = (Uint8*) &pxl[n];
			pxl_tmp = pxl8[0];
			pxl8[0] = pxl8[2];
			pxl8[2] = pxl_tmp;
		}
	}
 
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

