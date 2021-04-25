#include "meter.h"

meter_t* meter_init(uint8_t type, float scr_pos_x, float scr_pos_y) {
    
    meter_t* mtr = (meter_t*) malloc(sizeof(meter_t));
    
    mtr->value = 0;
    mtr->type = type;
    mtr->scr_pos_x = scr_pos_x;
    mtr->scr_pos_y = scr_pos_y;
    mtr->bg = 185; // 185 is the pink color used for transparency
    
    switch (type) {
    
        case METER_BEER:
            mtr->surf = surface_on_load("objects/meter_beer.bmp");
            break;
        case METER_POINTS:
            mtr->surf = surface_on_load("objects/meter_points.bmp");
            uint8_t* pxl = mtr->surf->pixels;
            printf("COLOR meter points: %d\n", pxl[0]);
            break;
        case METER_MOOD:
            mtr->surf = surface_on_load("objects/meter_mood.bmp");
            break;
        case METER_URIN:
            mtr->surf = surface_on_load("objects/meter_urin.bmp");
            break;
        case METER_ITEM:
            mtr->surf = surface_on_load("objects/meter_item.bmp");
            break;
    }
    
    return(mtr);
}

void meter_update(meter_t* mtr, int16_t value) {
    
    switch (mtr->type) {
        case METER_BEER:
            if (value < 0) {
                value = 0;
            } else if (value > 100) {
                value = 100;
            }
            break;
        case METER_MOOD:
            if (value < -50) {
                value = -50;
            } else if (value > 50) {
                value = 50;
            }
            break;
        case METER_URIN:
            if (value < 0) {
                value = 0;
            } else if (value > 100) {
                value = 100;
            }
            break;
    }
    mtr->value = value;
    
    uint8_t* pxl = (uint8_t*) mtr->surf->pixels;
    
    // bitmaps are stored as 32 bit blocks in memory. as we use
    // 8 bit per pixel, we have to account for additional junk
    // pixels that might be stored at the end of every row:
    uint32_t w_bmp;
    if (mtr->surf->w % 4 == 0) {
        w_bmp = mtr->surf->w;
    } else {
        w_bmp = mtr->surf->w + (4 - (mtr->surf->w % 4));
    }
    
    const uint32_t border = 4;
    
    // errase display:
    uint32_t start = border - 1;
    uint32_t end = w_bmp - border;
    uint32_t index;
    
    for (uint32_t x = start; x < end; x++) {
        
        for (uint32_t y = border; y < mtr->surf->h - border; y++) {
            
            index = (y * w_bmp) + x;
            if (pxl[index] != 0 && pxl[index] != 215) { // black, white
            //if (pxl[index] == color) {
                pxl[index] = mtr->bg;
            }
        }
    }

    // print graphical representation of value:
    uint8_t color = 0;
    
    switch (mtr->type) {
        case METER_BEER:
            color = 198;        // piss yellow
            start = border;
            end = start + value;
            break;
        case METER_MOOD:
            if (value > 0) {
                color = 30;     // green
                start = mtr->surf->w / 2;
                end = mtr->surf->w / 2 + value;
            } else {
                color = 180;    // red
                start = mtr->surf->w / 2 + value;
                end = mtr->surf->w / 2;
            }
            break;
        case METER_URIN:
            color = 198;
            start = border;
            end = start + value;
            break;
    }
    
    for (uint32_t x = start; x < end; x += 2) {
        
        for (uint32_t y = border; y < mtr->surf->h - border; y++) {
            
            index = (y * w_bmp) + x;
            if (pxl[index] != 0 && pxl[index] != 215) { // black, white
            //if (pxl[index] == mtr->bg) {
                pxl[index] = color;
            }
        }
    }
    
}

void meter_free(meter_t* mtr) {
    
    SDL_FreeSurface(mtr->surf);
    free(mtr);
}
