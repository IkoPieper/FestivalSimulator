#ifndef _SOUND_H_
    #define _SOUND_H_
 
#include <SDL2/SDL_mixer.h>
 
typedef struct sound sound_t;

struct sound {
    Mix_Chunk* samples;     // holds all short waveforms to be played
    uint32_t num_samples;   // number of short waveforms
    Mix_Music* music;       // to play songs of the bands. only a part
                            // of music will be stored in memory.
    char** songs;           // file names of the songs
    uint32_t num_songs;     // number of songs
    uint32_t n;             // current song
    float pos_x;            // position of song source (a.k.a. stage)
    float pos_y;
};

#define SOUND_COLLISION 0
#define SOUND_STEP 1


#endif
