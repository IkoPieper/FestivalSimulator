#ifndef _ON_CLEANUP_H_
    #define _ON_CLEANUP_H_

#include <SDL2/SDL.h>
#include "sound.h"
#include "object.h"
#include "tasks.h"
#include "groups.h"
#include "on_init.h"
#include "verletbox.h"

void on_cleanup(groups_t* grp, video_t* vid, sound_t* snd, 
    verletbox_t* vbox, bool* keys);

#endif
