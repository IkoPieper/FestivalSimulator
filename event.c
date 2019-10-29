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
        case SDLK_w:        keys[KEY_UP] = true; break;
		case SDLK_a:        keys[KEY_LEFT] = true; break;
		case SDLK_s:        keys[KEY_DOWN] = true; break;
		case SDLK_d:        keys[KEY_RIGHT] = true; break;
        case SDLK_UP:       keys[KEY_UP] = true; break;
		case SDLK_LEFT:     keys[KEY_LEFT] = true; break;
		case SDLK_DOWN:     keys[KEY_DOWN] = true; break;
		case SDLK_RIGHT:    keys[KEY_RIGHT] = true; break;
		case SDLK_ESCAPE:   keys[KEY_ESCAPE] = true; break;
		default: break;
	}
	
}

void on_key_up(bool* keys, SDLKey sym) {
	
	switch(sym) {
		case SDLK_w:        keys[KEY_UP] = false; break;
        case SDLK_a:        keys[KEY_LEFT] = false; break;
		case SDLK_s:        keys[KEY_DOWN] = false; break;
		case SDLK_d:        keys[KEY_RIGHT] = false; break;
        case SDLK_UP:       keys[KEY_UP] = false; break;
        case SDLK_LEFT:     keys[KEY_LEFT] = false; break;
		case SDLK_DOWN:     keys[KEY_DOWN] = false; break;
		case SDLK_RIGHT:    keys[KEY_RIGHT] = false; break;
		case SDLK_ESCAPE:   keys[KEY_ESCAPE] = false; break;
		default: break;
	}
	
}
