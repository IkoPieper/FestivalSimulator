#ifndef _ON_EVENT_H_
    #define _ON_EVENT_H_

#include <SDL/SDL.h>
#include <stdbool.h>

void on_event(SDL_Event* event, bool* keys);
void on_key_down(bool* keys, SDLKey sym);
void on_key_up(bool* keys, SDLKey sym);

#ifndef KEY_ESCAPE
	#define KEY_ESCAPE 0
#endif

#ifndef KEY_UP
	#define KEY_UP 1
#endif

#ifndef KEY_DOWN
	#define KEY_DOWN 2
#endif

#ifndef KEY_LEFT
	#define KEY_LEFT 3
#endif

#ifndef KEY_RIGHT
	#define KEY_RIGHT 4
#endif

#endif
