#ifndef _METER_H_
    #define _METER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include "surface.h"

typedef struct meter meter_t;

struct meter {
    int16_t value;      // value to display
    uint8_t type;       // meter type
    SDL_Surface* surf;
    uint8_t bg;         // pixel with background color
    int32_t scr_pos_x;  // pos in relation to top left corner of screen
    int32_t scr_pos_y;
};

meter_t* meter_init(uint8_t type, float scr_pos_x, float scr_pos_y);
void meter_free(meter_t* mtr);
void meter_update(meter_t* mtr, int16_t value);
int16_t meter_restrict_value(meter_t* mtr, int16_t value);
void meter_print_value_default(meter_t* mtr, uint32_t border);
void meter_print_value_item(meter_t* mtr, uint32_t border);

// meter types:
#define METER_BEER 0
#define METER_MOOD 1
#define METER_URIN 2
#define METER_POINTS 3
#define METER_ITEM 4

#endif
