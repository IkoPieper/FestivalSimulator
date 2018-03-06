#ifndef _ON_LOOP_H_
    #define _ON_LOOP_H_

#include <SDL/SDL.h>
#include <math.h>
#include "object.h"
#include "verletbox.h"

typedef struct n n_t;
struct n {
	float x;
	float y;
	float abs;
	float xp;
	float yp;
	float absp;
};

void on_loop(object_t* obj, verletbox_t* vbox, short* keys, unsigned long frame);
void on_loop_animations(object_t* obj, short* keys, unsigned long frame);
void on_loop_waypoints(object_t* obj, unsigned long frame);

#endif
