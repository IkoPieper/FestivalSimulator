#ifndef _ON_INIT_H_
    #define _ON_INIT_H_

#include "load_config.h"
#include <dirent.h>

object_t* on_init_objects(video_t* vid, float dt, uint8_t area);
bool on_init_background(object_t* obj, video_t* vid, uint8_t area);
bool on_init_hero(object_t* obj, video_t* vid);
bool on_init_buden(object_t* obj);
bool on_init_objects_config(object_t* obj, float dt, uint8_t area);
bool on_init_items(object_t* obj);

#endif
