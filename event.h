#ifndef _ON_EVENT_H_
    #define _ON_EVENT_H_

#include <SDL2/SDL.h>
#include <stdbool.h>

void on_event(SDL_Event* event, bool* keys);
void on_key_down(bool* keys, SDL_Event* event);
void on_key_up(bool* keys, SDL_Event* event);


#define KEY_ESCAPE 0
#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 3
#define KEY_RIGHT 4
#define KEY_SPACE 5
#define KEY_SHIFT 6
#define KEY_CTRL 7
#define KEY_FULLSCREEN 8

#endif
