#ifndef _ON_CLEANUP_H_
    #define _ON_CLEANUP_H_

#include <SDL2/SDL.h>
#include "sound.h"
#include "object.h"
#include "on_init.h"
#include "verletbox.h"

void on_cleanup(object_t* obj, video_t* vid, sound_t* snd, 
    verletbox_t* vbox, bool* keys);

#endif
