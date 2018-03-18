#include "event.h"

void on_event(SDL_Event* event, short* keys) {
	switch(event->type) {
		case SDL_KEYDOWN:
			on_key_down(keys, event->key.keysym.sym);
			break;
		case SDL_KEYUP:
			on_key_up(keys, event->key.keysym.sym);
			break;
		case SDL_QUIT: 
            break;
        default: 
			break;
	}
}


void on_key_down(short* keys, SDLKey sym) {
	
	switch(sym) {
		case SDLK_q: keys[SDLK_q] = 1; break;
		case SDLK_a: keys[SDLK_a] = 1; break;
		case SDLK_o: keys[SDLK_o] = 1; break;
		case SDLK_l: keys[SDLK_l] = 1; break;
		case SDLK_w: keys[SDLK_w] = 1; break;
		case SDLK_s: keys[SDLK_s] = 1; break;
		case SDLK_d: keys[SDLK_d] = 1; break;
		case SDLK_ESCAPE: keys[SDLK_ESCAPE] = 1; break;
		default: break;
	}
	
}

void on_key_up(short* keys, SDLKey sym) {
	
	switch(sym) {
		case SDLK_q: keys[SDLK_q] = 0; break;
		case SDLK_a: keys[SDLK_a] = 0; break;
		case SDLK_o: keys[SDLK_o] = 0; break;
		case SDLK_l: keys[SDLK_l] = 0; break;
		case SDLK_w: keys[SDLK_w] = 0; break;
		case SDLK_s: keys[SDLK_s] = 0; break;
		case SDLK_d: keys[SDLK_d] = 0; break;
		case SDLK_SPACE: keys[SDLK_SPACE] = 1; break;
		default: break;
	}
	
}
