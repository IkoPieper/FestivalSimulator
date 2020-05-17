#ifndef _FESTIVAL_H_
    #define _FESTIVAL_H_
 
#include <SDL2/SDL.h>
#include "object.h"
#include "verletbox.h"
#include "on_init.h"
#include "surface.h"
#include "event.h"
#include "on_start.h"
#include "on_loop.h"
#include "on_render.h"
#include "on_cleanup.h"

bool on_execute();
int main(int argc, char* argv[]);

#endif
