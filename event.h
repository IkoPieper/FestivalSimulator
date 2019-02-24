#ifndef _ON_EVENT_H_
    #define _ON_EVENT_H_

#include <SDL/SDL.h>
#include <stdbool.h>

void on_event(SDL_Event* event, bool* keys);
void on_key_down(bool* keys, SDLKey sym);
void on_key_up(bool* keys, SDLKey sym);

#endif
