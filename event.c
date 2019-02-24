#include "event.h"

void on_event(SDL_Event* event, bool* keys) {
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


void on_key_down(bool* keys, SDLKey sym) {
	
	switch(sym) {
		case SDLK_q: keys[SDLK_q] = true; break;
		case SDLK_a: keys[SDLK_a] = true; break;
		case SDLK_o: keys[SDLK_o] = true; break;
		case SDLK_l: keys[SDLK_l] = true; break;
		case SDLK_w: keys[SDLK_w] = true; break;
		case SDLK_s: keys[SDLK_s] = true; break;
		case SDLK_d: keys[SDLK_d] = true; break;
		case SDLK_ESCAPE: keys[SDLK_ESCAPE] = 1; break;
		default: break;
	}
	
}

void on_key_up(bool* keys, SDLKey sym) {
	
	switch(sym) {
		case SDLK_q: keys[SDLK_q] = false; break;
		case SDLK_a: keys[SDLK_a] = false; break;
		case SDLK_o: keys[SDLK_o] = false; break;
		case SDLK_l: keys[SDLK_l] = false; break;
		case SDLK_w: keys[SDLK_w] = false; break;
		case SDLK_s: keys[SDLK_s] = false; break;
		case SDLK_d: keys[SDLK_d] = false; break;
		case SDLK_SPACE: keys[SDLK_SPACE] = 1; break;
		default: break;
	}
	
}
