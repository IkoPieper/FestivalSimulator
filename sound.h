#ifndef _SOUND_H_
    #define _SOUND_H_
 
#include <SDL2/SDL_mixer.h>
 
typedef struct sound sound_t;

struct sound {
    uint32_t num_channels;  // number of channels
    Mix_Chunk** samples;    // holds all short waveforms to be played
    uint32_t num_samples;   // number of short waveforms
    Mix_Music* music;       // to play songs of the bands. only a part
                            // of music will be stored in memory.
    char** songs;           // file names of the songs
    uint32_t num_songs;     // number of songs
    uint32_t n;             // current song
    float pos_x;            // position of song source (a.k.a. stage)
    float pos_y;
};

void sound_free(sound_t* snd);
void sound_play_sample_distance(
    sound_t* snd, uint32_t id, float dist_x, float dist_y);
void sound_play_sample(sound_t* snd, uint32_t id, int32_t channel);
void sound_music_loop(sound_t* snd);
void sound_music_volume_distance(sound_t* snd, float pos_x, float pos_y);
int32_t sound_volume_distance(float dist_x, float dist_y);

#define SOUND_COLLISION 0
#define SOUND_STEP 1
#define SOUND_WATER_PISTOL 2

#define CHANNEL_HERO_COLLISION 0
#define CHANNEL_HERO_STEP 1
#define CHANNEL_HERO_ITEM 2
#define CHANNEL_HERO_OTHER 3
#define CHANNEL_MIN_OTHER 4

#endif
