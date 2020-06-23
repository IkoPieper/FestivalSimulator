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
#include "video.h"
#include "sound.h"
#include <dirent.h>
#include <math.h>
#include <stdbool.h>


sound_t* on_init_sound();
sound_t* on_init_sound_samples(sound_t* snd);
sound_t* on_init_sound_songs(sound_t* snd);
bool on_init_sound_supported(char* filename);
video_t* on_init_video(bool VSYNC);
object_t* on_init_objects(video_t* vid, float dt, uint8_t area);
groups_t* on_init_groups(object_t* obj);
bool on_init_background(object_t* obj, video_t* vid, uint8_t area);
bool on_init_hero(object_t* obj, video_t* vid);
bool on_init_buden(object_t* obj);
bool on_init_objects_config(object_t* obj, float dt, uint8_t area);
bool on_init_items(object_t* obj);

#endif
