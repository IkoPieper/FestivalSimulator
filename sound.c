#include "sound.h"

sound_t* sound_init() {
    
    // Set up the audio stream:
    int32_t result = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512);
    if (result < 0) {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        exit(-1);
    }

    sound_t* snd = (sound_t*) malloc(sizeof(sound_t));
    snd->num_channels = 16;
    
    result = Mix_AllocateChannels(snd->num_channels);
    if (result < 0) {
        fprintf(stderr, "Unable to allocate mixing channels: %s\n", 
            SDL_GetError());
        exit(-1);
    }
    
    snd = sound_init_samples(snd);
    snd = sound_init_songs(snd);
    
    return(snd);
}

// load audio samples as defined in sound.h:
sound_t* sound_init_samples(sound_t* snd) {
    
    snd->num_samples = 3; // keep this up to date!
    snd->samples = 
        (Mix_Chunk**) malloc(snd->num_samples * sizeof(Mix_Chunk*));
    
    Mix_Chunk* chunk;
    
    // sample 1:
    chunk = Mix_LoadWAV("samples/collision.wav");
    if(!chunk) {
        printf("Mix_LoadWAV: %s\n", Mix_GetError());
        chunk = NULL;
    }
    snd->samples[SOUND_COLLISION] = chunk;
    
    // sample 2:
    chunk = Mix_LoadWAV("samples/step.wav");
    if(!chunk) {
        printf("Mix_LoadWAV: %s\n", Mix_GetError());
        chunk = NULL;
    }
    snd->samples[SOUND_STEP] = chunk;
    
    // sample 3:
    chunk = Mix_LoadWAV("samples/water_pistol.wav");
    if(!chunk) {
        printf("Mix_LoadWAV: %s\n", Mix_GetError());
        chunk = NULL;
    }
    snd->samples[SOUND_WATER_PISTOL] = chunk;
    
    return(snd);
}

// automatically load a list of music files and load the first 
// audiofile for playback:
sound_t* sound_init_songs(sound_t* snd) {
    
    snd->pos_x = 745.0;
    snd->pos_y = -200.0;
    snd->n = 0;
    
    DIR* hdl_dir;
    struct dirent* dir;
    char path[256];
    
    hdl_dir = opendir("songs");
    
    // count files:
    uint32_t n = 0;
    while ((dir = readdir(hdl_dir)) != NULL) {
        if (sound_supported(dir->d_name)) {
            n++;
        }
    }
    
    if (n == 0) {
        printf("No supported sound files found in songs folder!\n");
        snd->num_songs = n;
        snd->songs = NULL;
        closedir(hdl_dir);
        return(snd);
    }
    
    snd->num_songs = n;
    snd->songs = (char**) malloc(n * sizeof(char*));
    
    rewinddir(hdl_dir);
    n = 0;
    while ((dir = readdir(hdl_dir)) != NULL) {
      
        if (sound_supported(dir->d_name)) {
            
            // build path to file:
            strncpy(path, "songs", 6);
            strncat(path, "/", 2);
            strncat(path, dir->d_name, 256-6-1);
            
            // save path:
            snd->songs[n] = (char*) malloc(256 * sizeof(char));
            strncpy(snd->songs[n], path, 256-1);
            
            n++;
        }
    }
    
    closedir(hdl_dir);
    
    for (n = 0; n < snd->num_songs; n++) {
        printf("song %d: %s\n", n, snd->songs[n]);
    }
    
    
    // load first song:
    if (!(snd->music = Mix_LoadMUS(snd->songs[0]))) {
        fprintf(stderr, "Error loading song %s!\n", snd->songs[0]);
    }
    
    Mix_PlayMusic(snd->music, 1);
    Mix_VolumeMusic(0);
    
    return(snd);
}

bool sound_supported(char* filename) {
    
    return(
        strcmp(filename, ".") && 
        strcmp(filename, "..") && (
            strstr(filename, ".wav") != NULL ||
            strstr(filename, ".WAV") != NULL ||
            strstr(filename, ".mp3") != NULL ||
            strstr(filename, ".MP3") != NULL ||
            strstr(filename, ".ogg") != NULL ||
            strstr(filename, ".OGG") != NULL ||
            strstr(filename, ".aiff") != NULL ||
            strstr(filename, ".AIFF") != NULL ||
            strstr(filename, ".flac") != NULL ||
            strstr(filename, ".FLAC") != NULL)
          );
}

void sound_free(sound_t* snd) {
    
    while(Mix_Init(0)) {
        Mix_Quit();
    }
    Mix_CloseAudio();
    
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
