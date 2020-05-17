#ifndef _ON_RENDER_H_
    #define _ON_RENDER_H_

#include "object.h"
#include "on_init.h"
#include "surface.h"
#include "meter.h"
#include "text.h"
#include "list.h"
#include <GL/gl.h>
#include <GL/glu.h>

void on_render(object_t* obj, video_t* vid);
void on_render_sort(object_t* obj);
list_t* render_blobb(list_t* blobb, list_t* current);
list_t* render_blobb_sort(list_t* blobb);
bool render_blobb_sort_iter(list_t* blobb);
void on_render_text(object_t* obj, video_t* vid);
void on_render_meters(object_t* obj, video_t* vid);
void on_render_item(object_t* obj, video_t* vid);

#endif
