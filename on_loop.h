#ifndef _ON_LOOP_H_
    #define _ON_LOOP_H_

#include <SDL/SDL.h>
#include <math.h>
#include "object.h"
#include "verletbox.h"
#include "event.h"

void on_loop(object_t* obj, verletbox_t* vbox, bool* keys, uint64_t frame);
void on_loop_animations(object_t* obj, bool* keys, uint64_t frame);
void on_loop_waypoints(object_t* obj, uint64_t frame);

#endif
