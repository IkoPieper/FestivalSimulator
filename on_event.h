#ifndef _ON_EVENT_H_
    #define _ON_EVENT_H_

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "constants.h"

void on_event(SDL_Event* event, bool* keys);
void on_key_down(bool* keys, SDL_Event* event);
void on_key_up(bool* keys, SDL_Event* event);

#endif
