#ifndef _VIDEO_H_
    #define _VIDEO_H_
 
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdbool.h>
 
typedef struct video video_t;

struct video {
	GLuint render_id;           // openGL id
    SDL_GLContext glcontext;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
    uint32_t fps;
    TTF_Font* font;
};

video_t* video_init(bool VSYNC);
void video_free(video_t* vid);
void video_render(video_t* vid);

#endif
