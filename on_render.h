#ifndef _ON_RENDER_H_
    #define _ON_RENDER_H_

#include "object.h"
#include "surface.h"
#include "text.h"
#include <GL/gl.h>
#include <GL/glu.h>

void on_render(object_t* obj);
void on_render_text(object_t* obj, object_t* obj_dsp);

#endif
