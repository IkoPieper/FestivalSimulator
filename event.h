#ifndef _ON_EVENT_H_
    #define _ON_EVENT_H_

#include <SDL/SDL.h>

void on_event(SDL_Event* event, short* keys);
void on_key_down(short* keys, SDLKey sym);
void on_key_up(short* keys, SDLKey sym);

#endif
