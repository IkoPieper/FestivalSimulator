#ifndef _VIDEO_H_
    #define _VIDEO_H_
 
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
 
typedef struct video video_t;

struct video {
	GLuint render_id;           // openGL id
    SDL_GLContext glcontext;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
};

#endif
