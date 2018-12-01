#ifndef _ON_INIT_H_
    #define _ON_INIT_H_

#include <math.h>
#include "surface.h"
#include "text.h"
#include "object.h"
#include "animation.h"
#include "own_config.h"  // TODO: check if still needed
#include "load_config.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <dirent.h>

object_t* on_init();
short on_init_surfdisplay(object_t* obj);
short on_init_background(object_t* obj);
short on_init_hero(object_t* obj);
short on_init_buden(object_t* obj);
short on_init_objects(object_t* obj);

#endif
