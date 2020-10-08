#ifndef _ON_INIT_H_
    #define _ON_INIT_H_

#include "surface.h"
#include "text.h"
#include "object.h"
#include "tasks.h"
#include "items.h"
#include "groups.h"
#include "animation.h"
#include "own_config.h"  // TODO: check if still needed
#include "load_config.h"
#include <dirent.h>
#include <math.h>
#include <stdbool.h>

object_t* on_init_objects(video_t* vid, float dt, uint8_t area);
bool on_init_background(object_t* obj, video_t* vid, uint8_t area);
bool on_init_hero(object_t* obj, video_t* vid);
bool on_init_buden(object_t* obj);
bool on_init_objects_config(object_t* obj, float dt, uint8_t area);
bool on_init_items(object_t* obj);

#endif
