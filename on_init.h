#ifndef _ON_INIT_H_
    #define _ON_INIT_H_

#include <math.h>
#include "surface.h"
#include "font.h"
#include "object.h"
#include "animation.h"
#include <GL/gl.h>
#include <GL/glu.h>

short on_init(object_t* obj);
short on_init_surfdisplay(object_t* obj);
short on_init_background(object_t* obj);
short on_init_hero(object_t* obj);
short on_init_score(object_t* obj);

#endif
