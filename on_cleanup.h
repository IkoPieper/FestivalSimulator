#ifndef _ON_CLEANUP_H_
    #define _ON_CLEANUP_H_

#include "sound.h"
#include "tasks.h"
#include "groups.h"
#include "verletbox.h"

void on_cleanup(
    groups_t* grp, video_t* vid, sound_t* snd, 
    verletbox_t* vbox, bool* keys);
void on_cleanup_area(
    groups_t* grp, sound_t* snd, verletbox_t* vbox);

#endif
