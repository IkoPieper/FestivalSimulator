#ifndef _COLLISION_H_
    #define _COLLISION_H_

//#include "datatypes.h"
#include "object.h"
#include "groups.h"
#include "verletbox.h"
#include <math.h>
#include <stdbool.h>

collision_t* collisions_add_to_object(object_t* obj, object_t* partner);
void collisions_free(object_t* obj);

void collisions(groups_t* grp, verletbox_t* vbox, float dt);

list_t* collisions_blobb(list_t* blobb, list_t* current);

void collisions_check(object_t* obj1, object_t* obj2, float dt);

bool collisions_detect_pixel_collision(
    uint8_t* pxl1, uint8_t* pxl2,
    int32_t x1_min, int32_t x1_max, 
    int32_t y1_min, int32_t y1_max, int32_t w1_bmp, int32_t w1, int32_t h1, 
    int32_t x2_min, 
    int32_t y2_min, int32_t w2_bmp, int32_t w2, int32_t h2);

void collisions_surface_vector(
    float* c1x, float* c1y, float* c2x, float* c2y,
    uint8_t* pxl1, uint8_t* pxl2,
    int32_t x1_min, int32_t x1_max, 
    int32_t y1_min, int32_t y1_max, int32_t w1_bmp, int32_t w1, int32_t h1, 
    int32_t x2_min, 
    int32_t y2_min, int32_t w2_bmp, int32_t w2, int32_t h2);

int32_t collisions_surface_vector_check(
    int32_t x_start, int32_t y_start, 
    float* c1x, float* c1y, float* c2x, float* c2y, 
    uint8_t* pxl1, uint8_t* pxl2,
    int32_t x1_min, int32_t x1_max, 
    int32_t y1_min, int32_t y1_max, int32_t w1_bmp, int32_t w1, int32_t h1,
    int32_t x2_min, 
    int32_t y2_min, int32_t w2_bmp, int32_t w2, int32_t h2);

bool collisions_pixels_shared(
    uint8_t* pxl1, uint8_t* pxl2, 
    int32_t x1, int32_t y1, 
    int32_t x1_min, int32_t y1_min, int32_t w1_bmp, 
    int32_t x2_min, int32_t y2_min, int32_t w2_bmp);

bool collisions_pixels_shared_protected(
    uint8_t* pxl1, uint8_t* pxl2, 
    int32_t x1, int32_t y1, int32_t x1_min, int32_t y1_min, 
    int32_t w1_bmp, int32_t w1, int32_t h1,
    int32_t x2, int32_t y2,  int32_t x2_min, int32_t y2_min, 
    int32_t w2_bmp, int32_t w2, int32_t h2);

bool collisions_pixels_empty(
    uint8_t* pxl1, uint8_t* pxl2, 
    int32_t x1, int32_t y1, 
    int32_t x1_min, int32_t y1_min, int32_t w1_bmp, int32_t w1, int32_t h1,
    int32_t x2_min, int32_t y2_min, int32_t w2_bmp, int32_t w2, int32_t h2);
    
bool collisions_pixel(
    int32_t x, int32_t y, uint8_t* pxl, int32_t w_bmp);
    
bool collisions_pixel_protected(
    int32_t x, int32_t y, uint8_t* pxl, int32_t w_bmp, int32_t w, int32_t h);

void collisions_impulse(
	object_t* obj1, object_t* obj2, float c1x, float c1y);

void collisions_update_render(object_t* obj, object_t* obj_b);

int8_t collisions_beam(
    object_t* obj, int32_t x_start, int32_t y_start, bool left_point);

typedef struct vector vector_t;
struct vector {
    int32_t x;
    int32_t y;
};

#endif
