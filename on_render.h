#ifndef _ON_RENDER_H_
    #define _ON_RENDER_H_

#include "object.h"
#include "surface.h"
#include "text.h"
#include <GL/gl.h>
#include <GL/glu.h>

void on_render(object_t* obj);
void on_render_sort(object_t* obj);
listobj_t* render_blobb(listobj_t* blobb, listobj_t* list);
listobj_t* render_blobb_sort(listobj_t* blobb);
listobj_t* render_blobb_sort_iter(listobj_t* blobb, bool* swapped);
void on_render_text(object_t* obj, object_t* obj_dsp);

#endif
