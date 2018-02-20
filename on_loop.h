#ifndef _ON_LOOP_H_
    #define _ON_LOOP_H_

#include <SDL/SDL.h>
#include <math.h>
#include "object.h"

typedef struct n n_t;
struct n {
	float x;
	float y;
	float abs;
	float xp;
	float yp;
	float absp;
};

void on_loop(object_t* obj, short* keys, unsigned long frame);
void on_loop_animations(object_t* obj, short* keys, unsigned long frame);

#endif
