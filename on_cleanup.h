#ifndef _ON_CLEANUP_H_
    #define _ON_CLEANUP_H_

#include <SDL2/SDL.h>
#include "object.h"
#include "on_init.h"
#include "verletbox.h"

void on_cleanup(object_t* obj, video_t* vid, verletbox_t* vbox, bool* keys);

#endif
