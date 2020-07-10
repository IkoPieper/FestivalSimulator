#include "event.h"

void on_event(SDL_Event* event, bool* keys) {
	switch(event->type) {
		case SDL_KEYDOWN:
			on_key_down(keys, event);
			break;
		case SDL_KEYUP:
			on_key_up(keys, event);
			break;
		case SDL_QUIT: 
            break;
        default: 
			break;
	}
}


void on_key_down(bool* keys, SDL_Event* event) {
	
	switch(event->key.keysym.sym) {
        case SDLK_w:        keys[KEY_UP] = true; break;
		case SDLK_a:        keys[KEY_LEFT] = true; break;
		case SDLK_s:        keys[KEY_DOWN] = true; break;
		case SDLK_d:        keys[KEY_RIGHT] = true; break;
        case SDLK_UP:       keys[KEY_UP] = true; break;
		case SDLK_LEFT:     keys[KEY_LEFT] = true; break;
		case SDLK_DOWN:     keys[KEY_DOWN] = true; break;
		case SDLK_RIGHT:    keys[KEY_RIGHT] = true; break;
		case SDLK_ESCAPE:   keys[KEY_ESCAPE] = true; break;
        case SDLK_SPACE:    keys[KEY_SPACE] = true; break;
        case SDLK_LCTRL:    keys[KEY_CTRL] = true; break;
        case SDLK_LSHIFT:   keys[KEY_SHIFT] = true; break;
        case SDLK_F4:       keys[KEY_FULLSCREEN] = true; break;
		default: break;
	}
	
}

void on_key_up(bool* keys, SDL_Event* event) {
	
	switch(event->key.keysym.sym) {
		case SDLK_w:        keys[KEY_UP] = false; break;
        case SDLK_a:        keys[KEY_LEFT] = false; break;
		case SDLK_s:        keys[KEY_DOWN] = false; break;
		case SDLK_d:        keys[KEY_RIGHT] = false; break;
        case SDLK_UP:       keys[KEY_UP] = false; break;
        case SDLK_LEFT:     keys[KEY_LEFT] = false; break;
		case SDLK_DOWN:     keys[KEY_DOWN] = false; break;
		case SDLK_RIGHT:    keys[KEY_RIGHT] = false; break;
		case SDLK_ESCAPE:   keys[KEY_ESCAPE] = false; break;
        case SDLK_SPACE:    keys[KEY_SPACE] = false; break;
        case SDLK_LCTRL:    keys[KEY_CTRL] = false; break;
        case SDLK_LSHIFT:   keys[KEY_SHIFT] = false; break;
        case SDLK_F4:       keys[KEY_FULLSCREEN] = false; break;
		default: break;
	}
	
}
