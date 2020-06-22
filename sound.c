#include "sound.h"

void sound_free(sound_t* snd) {
    if (snd->num_songs > 0) {
        
        Mix_FreeMusic(snd->music);
        
        for (uint32_t n = 0; n < snd->num_songs; n++) {
            free(snd->songs[n]);
        }
        free(snd->songs);
    }
    if (snd->num_samples > 0) {
        
        for (uint32_t n = 0; n < snd->num_samples; n++) {
            Mix_FreeChunk(snd->samples[n]);
        }
        free(snd->samples);
    }
    free(snd);
}    

void sound_play_sample_distance(
    sound_t* snd, uint32_t id, float dist_x, float dist_y) {
    
    if (dist_x * dist_x + dist_y * dist_y < 400000) {
        
        if (snd->samples[id] != NULL) {
            
            int32_t channel = CHANNEL_MIN_OTHER;
            while (channel < snd->num_channels && Mix_Playing(channel)) {
                channel++;
            }
            if (channel < snd->num_channels) {
                Mix_PlayChannel(channel, snd->samples[id], 0);
                int32_t volume = sound_volume_distance(dist_x, dist_y);
                Mix_Volume(channel, volume / 3);
            }
            
        }
    }
}

// currently only used for hero:
void sound_play_sample(sound_t* snd, uint32_t id, int32_t channel) {
    
    if (!Mix_Playing(channel)) {
        //Mix_HaltChannel(channel);
        Mix_PlayChannel(channel, snd->samples[id], 0);
        Mix_Volume(channel, MIX_MAX_VOLUME / 2);
    }
}

// select next song if the current one is finished.
void sound_music_loop(sound_t* snd) {

    if (snd->num_songs > 0 && !Mix_PlayingMusic()) {
        
        Mix_FreeMusic(snd->music);
            
        snd->n++;
        if (snd->n == snd->num_songs) {
            snd->n = 0;
        }
        if(!(snd->music = Mix_LoadMUS(snd->songs[snd->n]))) {
            fprintf(stderr, "Error loading song %s!\n", snd->songs[snd->n]);
            return;
        }
        Mix_PlayMusic(snd->music, 1);
    }
}


// set music volume according to distance to sound source.
// pos_x, pos_y are the position from which the sound is heard.
void sound_music_volume_distance(sound_t* snd, float pos_x, float pos_y) {

    if (snd->num_songs > 0) {
        float dist_x = pos_x - snd->pos_x;
        float dist_y = pos_y - snd->pos_y;
        Mix_VolumeMusic(sound_volume_distance(dist_x, dist_y));
    }
}

int32_t sound_volume_distance(float dist_x, float dist_y) {
    
    float dist = sqrtf(dist_x * dist_x + dist_y * dist_y);
    if (dist < 100.0) {
        dist = 100.0;
    }
    int32_t volume = 100.0 * (float) MIX_MAX_VOLUME / dist;
    if (volume < 1) {
        volume = 1;
    }
    
    return(volume);
}
