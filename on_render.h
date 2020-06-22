#ifndef _ON_RENDER_H_
    #define _ON_RENDER_H_

#include "object.h"
#include "groups.h"
#include "video.h"
#include "surface.h"
#include "meter.h"
#include "text.h"
#include "list.h"

void on_render(groups_t* grp, video_t* vid, float dt);
void on_render_sort(object_t* obj_first);
list_t* render_blobb(list_t* blobb, list_t* current);
list_t* render_blobb_sort(list_t* blobb);
void on_render_object(object_t* obj, video_t* vid);
void on_render_text(object_t* obj, video_t* vid);
void on_render_meters(object_t* obj, video_t* vid);
void on_render_item(object_t* hero, video_t* vid);
void on_render_object_id(video_t* vid, object_t* obj);

#endif
