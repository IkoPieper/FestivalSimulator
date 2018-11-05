#ifndef _ON_CLEANUP_H_
    #define _ON_CLEANUP_H_

#include <SDL/SDL.h>
#include "object.h"
#include "verletbox.h"

void on_cleanup(object_t* obj, verletbox_t* vbox, short* keys);

#endif
