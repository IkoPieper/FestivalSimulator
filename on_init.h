#ifndef _ON_INIT_H_
    #define _ON_INIT_H_

#include "surface.h"
#include "text.h"
#include "object.h"
#include "animation.h"
#include "own_config.h"  // TODO: check if still needed
#include "load_config.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <dirent.h>
#include <math.h>
#include <stdbool.h>

typedef struct video video_t;

struct video {
	GLuint render_id;           // openGL id
    SDL_GLContext glcontext;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
};

video_t* on_init_video();
object_t* on_init_objects(video_t* vid);
bool on_init_background(object_t* obj, video_t* vid);
bool on_init_hero(object_t* obj, video_t* vid);
bool on_init_buden(object_t* obj);
bool on_init_objects_config(object_t* obj);
bool on_init_items(object_t* obj);

#endif
