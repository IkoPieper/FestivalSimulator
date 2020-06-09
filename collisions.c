#include "collisions.h"

void collisions(object_t* obj, verletbox_t* vbox, float dt) {
	
    object_t* obj_first = object_get_first(obj);
    
	bool collision = false;
	uint32_t x, y, x2, y2, x2_min, y2_min, x2_max, y2_max;
	
	verletbox_update(vbox, obj);
	
	// check collisions with background first, 
	// because background is too big for verlet boxes:
	object_t* obj_bg = object_get(obj, OBJECT_BACKGROUND_ID);
	
	obj = obj_first;
			
	while (obj != NULL) {
        
        obj->vel_lock = false;
        
		if (obj->has_moved && 
            (!obj->disable_collision || !obj->disable_render)) {
			
			collision = collisions_check(obj_bg, obj, dt);
				
			if (collision == false) {
				object_remove_collision(obj_bg, obj);
				object_remove_collision(obj, obj_bg);
			}
		
		}
		
		obj = obj->next_object;
	}
	
	// iterate over verlet boxes:
	object_t* obj_b = NULL;
	
	for (x = 0; x < vbox->num_w; x++) {
		for (y = 0; y < vbox->num_h; y++) {
			
			obj = verletbox_get_first_object(vbox->boxes[x][y]);
			
			while (obj != NULL) {
				
					
                // iterate over verlet box of obj and surrounding ones:
                if (y == 0) {
                    y2_min = 0;
                } else {
                    y2_min = y;
                }
                if (y == vbox->num_h-1) {
                    y2_max = vbox->num_h-1;
                } else {
                    y2_max = y + 1;
                }
                if (x == vbox->num_w-1) {
                    x2_max = vbox->num_w-1;
                } else {
                    x2_max = x + 1;
                }
                
                for (y2 = y2_min; y2 <= y2_max; y2++) {
                    
                    if (x == 0) {
                        x2_min = 0;
                    } else {
                        if (y == y2) {
                            x2_min = x; // omit previous box already iterated
                        } else {
                            x2_min = x - 1;
                        }
                    }
                
                    for (x2 = x2_min; x2 <= x2_max; x2++) {
                    
                        if (x2 == x && y2 == y) {   // same vbox as obj
                            obj_b = obj->next_vbox; // only objects after obj
                        } else {                    // neighbour vbox
                            obj_b = verletbox_get_first_object(vbox->boxes[x2][y2]);
                        }
                        
                        // interactions with other objects in selected vbox:
                        while (obj_b != NULL) {
                            
                            collision = collisions_check(obj, obj_b, dt);
                            
                            if (collision == false) {
                                object_remove_collision(obj, obj_b);
                                object_remove_collision(obj_b, obj);
                            }
                            
                            // get next object obj_b:
                            obj_b = obj_b->next_vbox;
                        }
                    }
                }
				
				// get next object obj:
				obj = obj->next_vbox;
			}
			
		}					
	}
    
    // calculate impulses (new velocites):
    
    obj = obj_first;
    while (obj != NULL) {
        
        obj->impulse_calculated = false;
        
        obj = obj->next_object;
    }
    
    obj = obj_first;
    while (obj != NULL) {
        
        if (obj->col != NULL && !obj->impulse_calculated) {
            
            // bei objekt mit hoechsten |v| anfangen:
            printf("\nFind object with highest |v| in collisions():\n");
            
            printf("Start with obj->id: %d\n", obj->id);
            
            object_t* obj_vel_max = obj;
            float vel_x = obj->vel_x;
            float vel_y = obj->vel_y;
            float vel = sqrtf(vel_x * vel_x + vel_y * vel_y);
            
            printf("vel_x: %f\n", vel_x);
            printf("vel_y: %f\n", vel_y);
            printf("vel: %f\n", vel);
            
            float vel_max = vel;
            
            list_t* lst = NULL;
            lst = collisions_blobb(lst, obj->col);
            lst = get_first(lst);
            while (lst != NULL) {
                
                collision_t* col = (collision_t*) lst->entry;
                
                printf("col->partner->id: %d\n", col->partner->id);
                vel_x = col->partner->vel_x;
                vel_y = col->partner->vel_y;
                vel = sqrtf(vel_x * vel_x + vel_y * vel_y);
                printf("vel_x: %f\n", vel_x);
                printf("vel_y: %f\n", vel_y);
                printf("vel: %f\n", vel);
                
                if (vel > vel_max) {
                    vel_max = vel;
                    obj_vel_max = col->partner;
                }
                
                lst = lst->next;
            }
            
            printf("obj_vel_max->id: %d\n\n", obj_vel_max->id);
            // impuls mit allen berechnen, dessen c-vektor entgegen-
            // gesetzt von v zeigt. bei den anderen wieder: impuls
            // mit allen berechnen dessen c-vektor entgegengesetzt
            // vom neuen v zeigen (rekursiv):
            collisions_impulse_chain(obj_vel_max, obj_vel_max->id, 0);
        }
        
        obj = obj->next_object;
    }
    
    // push objects already into the new direction:
    obj = obj_first;
    while (obj != NULL) {
        if (obj->col != NULL) {
            obj->pos_x += obj->vel_x;
            obj->pos_y += obj->vel_y;
        }
        obj = obj->next_object;
    }
}

// collect objects that collide and put them into a "blobb":
list_t* collisions_blobb(list_t* blobb, list_t* current) {
    
    current = get_first(current);
    while (current != NULL) {
        
        collision_t* col = (collision_t*) current->entry;
        
        if (!col->is_in_blobb) {
            
            blobb = create_before(blobb, (void*) col, 0);
            col->is_in_blobb = true;
            
            if (col->partner->col != NULL) {
                blobb = collisions_blobb(blobb, col->partner->col);
            } else {
                printf("col != NULL in collisions_blobb()! How is that possible?\n");
            }
        }
        current = current->next;
    }
    
    return(blobb);
}

bool collisions_check(object_t* obj1, object_t* obj2, float dt) {
	
	bool collision = false;
	
	// boundary boxes base positions:
	int32_t x01 = (int32_t) obj1->pos_x;
	int32_t y01 = (int32_t) obj1->pos_y;
	int32_t x02 = (int32_t) obj2->pos_x;
	int32_t y02 = (int32_t) obj2->pos_y;
	
	int32_t xw1 = x01 + obj1->surface->w;
	int32_t yh1 = y01 + obj1->surface->h;
	int32_t xw2 = x02 + obj2->surface->w;
	int32_t yh2 = y02 + obj2->surface->h;
	
	// check collision of surfaces first:
    if (obj1->id == OBJECT_BACKGROUND_ID) {
        
        collision = true;
        
    } else if (obj2->id == OBJECT_BACKGROUND_ID) {
        
        collision = true;
        
    } else {
        
        // check for collision of boundary boxes:
        if (xw2 > x01 && x02 < xw1 && 
            yh2 > y01 && y02 < yh1) {
            
            collision = true;
            
        }
    }
	
	// update render list:
	if (collision &&
        (!obj1->disable_render && !obj2->disable_render) &&
		obj1->id != OBJECT_BACKGROUND_ID &&
		obj2->id != OBJECT_BACKGROUND_ID) {
		
		collisions_update_render(obj1, obj2);
	}
	
    if (obj1->disable_collision || obj2->disable_collision ||
        (!obj1->can_move && !obj2->can_move)) {
        collision = false;
    }
    
	// check for pixel wise collision:
	if (collision && (obj1->has_moved || obj2->has_moved)) {
        
		collision = false;
		
		collision_t* col1 = NULL;
		collision_t* col2 = NULL;
		uint32_t w1_bmp = obj1->wall->w_bmp;
		uint32_t w2_bmp = obj2->wall->w_bmp;
		
		x01 = (int32_t) obj1->pos_x + obj1->wall->x;
		y01 = (int32_t) obj1->pos_y + obj1->wall->y;
		x02 = (int32_t) obj2->pos_x + obj2->wall->x;
		y02 = (int32_t) obj2->pos_y + obj2->wall->y;

		int32_t w1 = obj1->wall->w;
		int32_t h1 = obj1->wall->h;
		int32_t w2 = obj2->wall->w;
		int32_t h2 = obj2->wall->h;
		
		xw1 = x01 + w1;
		yh1 = y01 + h1;
		xw2 = x02 + w2;
		yh2 = y02 + h2;
		
		// current position in overlapping area:
		int32_t x1_min = 0;
		int32_t x1_max = 0;
		int32_t y1_min = 0;
		int32_t y1_max = 0;
		int32_t x2_min = 0;
		int32_t y2_min = 0;
		
		// overlapping area of boundary boxes:
		if (x01 < x02) {
			x1_min = x02 - x01;
			x2_min = 0;
		} else {
			x1_min = 0;
			x2_min = x01 - x02;
		}
		if (y01 < y02) {
			y1_min = y02 - y01;
			y2_min = 0;
		} else {
			y1_min = 0;
			y2_min = y01 - y02;
		}
		if (xw1 < xw2) {
			x1_max = w1;
		} else {
			x1_max = w1 - (xw1 - xw2);
		}
		if (yh1 < yh2) {
			y1_max = h1;
		} else {
			y1_max = h1 - (yh1 - yh2);
		}
		
        if (x1_max < x1_min || y1_max < y1_min) {
            return(false);
        }
        
        
        collision = collisions_detect_pixel_collision(
            obj1->wall->pxl, obj2->wall->pxl, 
            x1_min, x1_max, y1_min, y1_max, w1_bmp, w1, h1,
            x2_min, y2_min, w2_bmp, w2, h2);
            
        if (collision) {
            col1 = object_add_collision(obj1, obj2);
            col2 = object_add_collision(obj2, obj1);
        } else {
            return(false);
        }
        
        printf("\n---------------------- collisions_check() pixel collision ----------------------\n");
        printf("obj1->id: %d, obj2->id: %d\n", obj1->id, obj2->id); 
        
        // find the two pixels that are zero for both objects. the line 
        // between them is the collision surface (90° to c).
        float c1x = 0.0;
        float c1y = 0.0;
        float c2x = 0.0;
        float c2y = 0.0;
        
        collisions_surface_vector(
            &c1x, &c1y, &c2x, &c2y, 
            obj1->wall->pxl, obj2->wall->pxl,
            x1_min, x1_max, y1_min, y1_max, w1_bmp, w1, h1,
            x2_min, y2_min, w2_bmp, w2, h2);
        
        if (c1x == 666.0) {
            // obj1 is buried inside obj2:
            c1x = obj1->vel_x;
            c1y = obj1->vel_y;
            float norm = sqrtf(c1x * c1x + c1y * c1y);
            c1x /= norm;
            c1y /= norm;
            c2x = -c1x;
            c2y = -c1y;
        } else if (c2x == 666.0) {
            // obj2 is buried inside obj1:
            c2x = obj2->vel_x;
            c2y = obj2->vel_y;
            float norm = sqrtf(c2x * c2x + c2y * c2y);
            c2x /= norm;
            c2y /= norm;
            c1x = -c2x;
            c1y = -c2y;
        }
        
		if (collision) {
            
			// update direction of collision:
			col1->c_x = c1x;
			col1->c_y = c1y;
			col2->c_x = c2x;
			col2->c_y = c2y;
			
            // forbid all velocity changes:
            obj1->vel_lock = true;	
			obj2->vel_lock = true;
		}
	}
	
	return(collision);
}

bool collisions_detect_pixel_collision(
    uint8_t* pxl1, uint8_t* pxl2,
    int32_t x1_min, int32_t x1_max, 
    int32_t y1_min, int32_t y1_max, int32_t w1_bmp, int32_t w1, int32_t h1, 
    int32_t x2_min, 
    int32_t y2_min, int32_t w2_bmp, int32_t w2, int32_t h2) {
    
    int32_t x, x_min, x_max;
    int32_t y, y_min, y_max;
    int32_t x2;
    int32_t y2;
    bool pixel_not_found;
    
    // increase search area by 1 pixel:
    x_min = x1_min - 1;
    x_max = x1_max + 1;
    y_min = y1_min - 1;
    y_max = y1_max + 1;
    
    // find a pixel:
    y = y_min;
    y2 = y2_min + y - y1_min;
    pixel_not_found = true;
    while (y < y_max && pixel_not_found) {
        x = x_min;
        x2 = x2_min + x - x1_min;
        while (x < x_max && pixel_not_found) {
            if (collisions_pixels_shared_protected(
                    pxl1, pxl2, x, y, x1_min, y1_min, w1_bmp, w1, h1,
                    x2, y2, x2_min, y2_min, w2_bmp, w2, h2)) {
                pixel_not_found = false;
                return(true);
            } 
            x++;
            x2++;
        }
        y++;
        y2++;
    }
    
    return(false);
}

void collisions_surface_vector(
    float* c1x, float* c1y, float* c2x, float* c2y,
    uint8_t* pxl1, uint8_t* pxl2,
    int32_t x1_min, int32_t x1_max, 
    int32_t y1_min, int32_t y1_max, int32_t w1_bmp, int32_t w1, int32_t h1,
    int32_t x2_min, 
    int32_t y2_min, int32_t w2_bmp, int32_t w2, int32_t h2) {
    
    int32_t x1_found, y1_found, x2_found, y2_found;
    
    printf("\ncollisions_surface_vector()\n");
    
    // find cutpoints.
    // first search for pixels which are empty for both objects but
    // have pixels of object 1 and object 2 as neighbours:
    int32_t num_pixel_found = 0;
    
    int32_t x, y, x2, y2;
    int32_t x_min, y_min, x_max, y_max;
    
    list_t* cutpoints = NULL;
    list_t* lst = NULL;
    list_t* lst2 = NULL;
    
    vector_t* vec;
    vector_t* vec2;
    
    // increase search area by 1 pixel:
    x_min = x1_min - 1;
    x_max = x1_max + 1;
    y_min = y1_min - 1;
    y_max = y1_max + 1;
    
    printf("\nx_min: %d\n", x_min);
    printf("x_max: %d\n", x_max);
    printf("y_min: %d\n", y_min);
    printf("y_max: %d\n", y_max);
    
    // print for debugging:
    bool spxl1, spxl2;
    
    for (y = y_min; y < y_max; y++) {
        
        printf("%3d:", y);
        
        for (x = x_min; x < x_max; x++) {
            
            spxl1 = collisions_pixel_protected(x, y, pxl1, w1_bmp, w1, h1);
            
            x2 = x2_min + x - x1_min;
            y2 = y2_min + y - y1_min;
            
            spxl2 = collisions_pixel_protected(x2, y2, pxl2, w2_bmp, w2, h2);
            
            if (spxl1 && spxl2) {
                printf(" *");
            } else if (spxl1) {
                printf(" 1");
            } else if (spxl2) {
                printf(" 2");
            } else {
                printf(" 0");
            }
        }
        printf("\n");
    }
    printf("\n");
    
    
    bool pxl1_found;
    bool pxl2_found;
    for (y = y_min; y < y_max; y++) {
        for (x = x_min; x < x_max; x++) {
            
            pxl1_found = false;
            pxl2_found = false;
            
            if (collisions_pixels_empty(
                pxl1, pxl2, x, y, x1_min, y1_min, w1_bmp, w1, h1,
                x2_min, y2_min, w2_bmp, w2, h2)) {
                
                // search neighbours:
                int32_t yy = y - 1;
                int32_t yy_max = y + 2;
                int32_t xx_max = x + 2;
                int32_t yy2 = y2_min + yy - y1_min;
                while (yy < yy_max && (!pxl1_found || !pxl2_found)) {
                    int32_t xx = x - 1;
                    int32_t xx2 = x2_min + xx - x1_min;
                    while (xx < xx_max && (!pxl1_found || !pxl2_found)) {
                        if (collisions_pixel_protected(
                            xx, yy, pxl1, w1_bmp, w1, h1)) {
                            pxl1_found = true;
                        }
                        if (collisions_pixel_protected(
                            xx2, yy2, pxl2, w2_bmp, w2, h2)) {
                            pxl2_found = true;
                        }
                        xx++;
                        xx2++;
                    }
                    yy++;
                    yy2++;
                }
                
                if (pxl1_found && pxl2_found) {
                    
                    printf("adding vector:\n");
                    printf("x: %d\n", x);
                    printf("y: %d\n", y);
                    
                    vec = (vector_t*) malloc(sizeof(vector_t));
                    vec->x = x;
                    vec->y = y;
                    cutpoints = create_after(cutpoints, (void*) vec, 0);
                    
                    num_pixel_found++;
                }
            }
        }
    }
    
    // find cutpoints with highest distance:
    if (num_pixel_found > 2) {
        printf("more than 2 pixels found!\n");
        printf("trying to find the pixels with highest distance!\n");
    }
    
    if (num_pixel_found > 1) {
        
        float dist_abs_max = 0.0;
        float dist_abs = 0.0;
        float dist_x = 0.0;
        float dist_y = 0.0;
        vector_t* vec1_found = NULL;
        vector_t* vec2_found = NULL;
        
        lst = get_first(cutpoints);
        while (lst != NULL) {
            
            vec = (vector_t*) lst->entry;
            
            lst2 = get_first(cutpoints);
            while (lst2 != NULL) {
                
                vec2 = (vector_t*) lst2->entry;
                
                dist_x = vec2->x - vec->x;
                dist_y = vec2->y - vec->y;
                dist_abs = dist_x * dist_x + dist_y * dist_y;
                
                if (dist_abs > dist_abs_max) {
                    printf("higher distance found:\n");
                    printf("dist_abs: %f\n", dist_abs);
                    
                    dist_abs_max = dist_abs;
                    
                    vec1_found = vec;
                    vec2_found = vec2;
                }
                
                lst2 = lst2->next;
            }
            
            lst = lst->next;
        }
        
        x1_found = vec1_found->x;
        y1_found = vec1_found->y;
        x2_found = vec2_found->x;
        y2_found = vec2_found->y;
        
        num_pixel_found = 2;
    }
    
    // FREE:
    lst = get_first(cutpoints);
    while (lst != NULL) {
        vec = (vector_t*) lst->entry;
        free(vec);
        lst = lst->next;
    }
    delete_all(cutpoints);
    
    
    if (num_pixel_found == 2) {
        printf("Num pixels found == 2\n");
        printf("x1_found: %d\n", x1_found);
        printf("y1_found: %d\n", y1_found);
        printf("x2_found: %d\n", x2_found);
        printf("y2_found: %d\n", y2_found);
        // surface vector:
        *c1x = x2_found - x1_found;
        *c1y = y2_found - y1_found;
        printf("c1x: %f\n", *c1x);
        printf("c1y: %f\n", *c1y);
        
        // collision vector is surface vector turned by 90°:
        float tmp = *c1x;
        *c1x = -*c1y;
        *c1y = tmp;
        float norm = sqrtf(*c1x * *c1x + *c1y * *c1y);
        printf("norm: %f\n", norm);
        *c1x /= norm;
        *c1y /= norm;
        *c2x = -*c1x;
        *c2y = -*c1y;
        
        printf("Before correction:\n");
        printf("c1x: %f\n", *c1x);
        printf("c1y: %f\n", *c1y);
        printf("c2x: %f\n", *c2x);
        printf("c2y: %f\n", *c2y);
        
        // correct for the right direction:
        // point from the middle of surface vector on the frame we have 
        // just searched:
        float dc1x = *c1x;
        float dc1y = *c1y;
        float dc2x = *c2x;
        float dc2y = *c2y;
        int32_t x_start = x1_found + (x2_found - x1_found) / 2;
        int32_t y_start = y1_found + (y2_found - y1_found) / 2;
        
        printf("start correction at:\n");
        printf("x_start: %d\n", x_start);
        printf("y_start: %d\n", y_start);
        
        
        int32_t step = 0;
        int32_t steps = 20;
        int32_t score = 0;
        while (step < steps) {
            
            // direction of c1:
            dc1x += *c1x;
            dc1y += *c1y;
            x = x_start + (int32_t) dc1x;
            y = y_start + (int32_t) dc1y;
            x2 = x2_min + x - x1_min;
            y2 = y2_min + y - y1_min;
            
            pxl1_found = collisions_pixel_protected(
                x, y, pxl1, w1_bmp, w1, h1);
            pxl2_found = collisions_pixel_protected(
                x2, y2, pxl2, w2_bmp, w2, h2);
            
            if (pxl2_found && !pxl1_found) {
                // c1 points on object 2, which is correct.
                score++;
            } else if (pxl1_found && !pxl2_found) {
                // c1 points on its own object.
                score--;
            }
            
            step++;
        }
        
        printf("score for c1: %d\n", score);
        
        step = 0;
        while (step < steps) {
            
            // direction of c2:
            dc2x += *c2x;
            dc2y += *c2y;
            x = x_start + (int32_t) dc2x;
            y = y_start + (int32_t) dc2y;
            x2 = x2_min + x - x1_min;
            y2 = y2_min + y - y1_min;
            
            pxl1_found = collisions_pixel_protected(
                x, y, pxl1, w1_bmp, w1, h1);
            pxl2_found = collisions_pixel_protected(
                x2, y2, pxl2, w2_bmp, w2, h2);
            
            if (pxl1_found && !pxl2_found) {
                // c2 points on object 1, which is correct.
                score++;
            } else if (pxl2_found && !pxl1_found) {
                // c2 points on its own object.
                score--;
            }
            
            step++;
        }
        
        printf("score: %d\n", score);
        
        if (score < 0) {
            // switch c1 and c2:
            dc1x = *c1x;
            dc1y = *c1y;
            *c1x = *c2x;
            *c1y = *c2y;
            *c2x = dc1x;
            *c2y = dc1y;
        }
        
        
        printf("After correction:\n");
        printf("c1x: %f\n", *c1x);
        printf("c1y: %f\n", *c1y);
        printf("c2x: %f\n", *c2x);
        printf("c2y: %f\n", *c2y);

    } else { // only num_pixels_found < 2 possible
        // one object is buried deeper than 1 pixel inside the other.
        
        if (x_min == -1) {
            // object 1 is buried
            printf("\n obj1 is buried inside of obj2!\n");
            *c1x = 666.0;
            *c1y = 666.0;
            *c2x = 0.0;
            *c2y = 0.0;
        } else {
            // object 2 is buried
            printf("\n obj2 is buried inside of obj1!\n");
            *c1x = 0.0;
            *c1y = 0.0;
            *c2x = 666.0;
            *c2y = 666.0;
        }
    }
}

bool collisions_pixels_shared_protected(
    uint8_t* pxl1, uint8_t* pxl2, 
    int32_t x1, int32_t y1, int32_t x1_min, int32_t y1_min, 
    int32_t w1_bmp, int32_t w1, int32_t h1,
    int32_t x2, int32_t y2,  int32_t x2_min, int32_t y2_min, 
    int32_t w2_bmp, int32_t w2, int32_t h2) {
    
    if (collisions_pixel_protected(x1, y1, pxl1, w1_bmp, w1, h1)) {
        
        return(collisions_pixel_protected(x2, y2, pxl2, w2_bmp, w2, h2));
    }
    
    return(false);
}

bool collisions_pixels_empty(
    uint8_t* pxl1, uint8_t* pxl2, 
    int32_t x1, int32_t y1, 
    int32_t x1_min, int32_t y1_min, int32_t w1_bmp, int32_t w1, int32_t h1,
    int32_t x2_min, int32_t y2_min, int32_t w2_bmp, int32_t w2, int32_t h2) {
    
    if (!collisions_pixel_protected(x1, y1, pxl1, w1_bmp, w1, h1)) {
        
        int32_t x2 = x2_min + x1 - x1_min;
        int32_t y2 = y2_min + y1 - y1_min;
        
        return(!collisions_pixel_protected(x2, y2, pxl2, w2_bmp, w2, h2));
    }
    
    return(false);
}

bool collisions_pixel(
    int32_t x, int32_t y, uint8_t* pxl, int32_t w_bmp) {
    
    if (pxl != NULL) {
        return(pxl[(y * w_bmp) + x]);
    } else { // use rectangular boundaries
        return(1);
    }
}

bool collisions_pixel_protected(
    int32_t x, int32_t y, uint8_t* pxl, int32_t w_bmp, int32_t w, int32_t h) {
    
    if (x < 0 || x >= w || y < 0 || y >= h) {
        return(0);
    } else {
        if (pxl != NULL) {
            return(pxl[(y * w_bmp) + x]);
        } else { // use rectangular boundaries
            return(1);
        }
    }
}

// impuls mit allen berechnen, dessen c-vektor entgegen-
// gesetzt von v zeigt. bei den anderen wieder: impuls
// mit allen berechnen dessen c-vektor entgegengesetzt
// vom neuen v zeigen (rekursiv):
void collisions_impulse_chain(
    object_t* obj, uint32_t called_by_obj_id, uint32_t depth) {
    
    printf("\ncollisions_impulse_chain()\n");
    printf("obj->id: %d\n", obj->id);
    printf("depth: %d\n", depth);
    depth++;
    
    list_t* lst = obj->col;
    lst = get_first(lst);
    
    while (lst != NULL) {
        
        collision_t* col = (collision_t*) lst->entry;
        
        if (col->partner->id != called_by_obj_id) {
        
            list_t* lst_partner = col->partner->col;
            lst_partner = find_id(lst_partner, obj->id);
    
            collision_t* col_partner = (collision_t*) lst_partner->entry;
            
            printf("col->partner->id: %d\n", col->partner->id);
            
            obj->impulse_calculated = true;
            col->partner->impulse_calculated = true;
            
            float vx = obj->vel_x;
            float vy = obj->vel_y;
            if (vx != 0.0 || vy != 0.0) {
                float norm = sqrtf(vx * vx + vy * vy);
                vx = vx / norm;
                vy = vy / norm;
            
                printf("vx: %f\n", vx);
                printf("vy: %f\n", vy);
                printf("obj->vel_x: %f\n", obj->vel_x);
                printf("obj->vel_y: %f\n", obj->vel_y);
                
                // collision vector of partner == - own collision vector
                float cx = col_partner->c_x;
                float cy = col_partner->c_y;
                printf("col_partner->c_x: %f\n", col_partner->c_x);
                printf("col_partner->c_y: %f\n", col_partner->c_y);
                printf("col->partner->vel_x: %f\n", col->partner->vel_x);
                printf("col->partner->vel_y: %f\n", col->partner->vel_y);
                
                
                // if angle not smaller than 90°:
                if (!((cx < -vy && cy < -vx) || (cx > -vy && cy > -vx))) {
                    
                    collisions_impulse(obj, col->partner, col, col_partner);
                    printf("After collisions_impulse():\n");
                    printf("obj->vel_x: %f\n", obj->vel_x);
                    printf("obj->vel_y: %f\n", obj->vel_y);
                    printf("col->partner->vel_x: %f\n", col->partner->vel_x);
                    printf("col->partner->vel_y: %f\n", col->partner->vel_y);
                    
                    collisions_impulse_chain(col->partner, obj->id, depth);
                }
            }
        }
        lst = lst->next;
    }
}

void collisions_impulse(
	object_t* obj1, object_t* obj2, 
	collision_t* col1, collision_t* col2) {
	
	float v1n_pre = 0.0;
	float v2n_pre = 0.0;
	float v1n = 0.0;
	float v2n = 0.0;
	float v1r = 0.0;
	float v2r = 0.0;
	
	float m1 = obj1->mass;
	float m2 = obj2->mass;
	float c1x = col1->c_x;
	float c1y = col1->c_y;
	float c2x = col2->c_x;
	float c2y = col2->c_y;
	float v1x = 0.0;
	float v1y = 0.0;
	float v2x = 0.0;
	float v2y = 0.0;
	/*float tmp = 0.0;
	float x12 = (float) (obj2->scr_pos_x - obj1->scr_pos_x);
	float y12 = (float) (obj2->scr_pos_y - obj1->scr_pos_y);*/
	
	if (obj1->can_move) {
		v1x = obj1->vel_x;
		v1y = obj1->vel_y;
	}
	
	if (obj2->can_move) {
		v2x = obj2->vel_x;
		v2y = obj2->vel_y;
	}
	
	// make shure surface vectors point directly toward each other:
	/*if (obj2->can_move && obj1->can_move) {
		if (c1x != -c2x && c1y != c2y) {
			tmp = -(c2y + c1y) / (c1x + c2x);
			if (tmp != 0.0) {
				c1x_tmp = c1x;
				c1y_tmp = c1y;
				c1x = tmp / sqrtf(1.0 + tmp * tmp);
				c1y = c1x / tmp;
				if (c1x * c1x_tmp + c1y * c1y_tmp < 0.0) { // > 90 °
					c1x = -c1x;
					c1y = -c1y;
				}
				c2x = -c1x;
				c2y = -c1y;
				
			}
		}
	}*/	
	
	// velocity in direction of impulse:
	v1n_pre = c1x * v1x + c1y * v1y;
	v2n_pre = c1x * v2x + c1y * v2y;
	
	// impulse:
    if (!obj2->can_move) {
        v1n = -v1n_pre;
    } else if (!obj1->can_move) {
        v2n = -v2n_pre;
    } else {
        v1n = 2.0 * (m1 * v1n_pre + m2 * v2n_pre) / (m1 + m2) - v1n_pre;
        v2n = 2.0 * (m1 * v1n_pre + m2 * v2n_pre) / (m1 + m2) - v2n_pre;
    }
	
	// in 2D an object with smaller velocity can accelerate an object 
	// with higher velocity. To solve this: switch v1n and v2n depending
	// on object positions relative to the hit location:
	/*if (obj2->can_move && obj1->can_move) {
		if (x12 * c1x + y12 * c1y < 0.0) {
			tmp = v1n;
			v1n = v2n;
			v2n = tmp;
		}
        printf("VELOCITIES v1n and v2n SWITCHED!!!\n");
        printf("obj1->id: %d, obj2->id: %d\n", obj1->id, obj2->id);
	}*/
	
	
	if (obj1->can_move) {
        // velocity vertically to impulse direction:
        v1r = c1y * v1x - c1x * v1y;
        // final velocities:
		obj1->vel_x = (c1x * v1n + c1y * v1r) * 0.95;
		obj1->vel_y = (c1y * v1n - c1x * v1r) * 0.95;
	}
	
	if (obj2->can_move) {
        v2r = c2y * v2x - c2x * v2y;
		obj2->vel_x = (c1x * v2n + c2y * v2r) * 0.95;
		obj2->vel_y = (c1y * v2n - c2x * v2r) * 0.95;
	}
}

void collisions_update_render(object_t* obj1, object_t* obj2) {
	
    if (obj1->render_early || obj2->render_early) {
        return;
    }
    
	float x01 = obj1->pos_x + (float) obj1->wall->x;
	float y01 = obj1->pos_y + (float) obj1->wall->y;
	float x02 = obj2->pos_x + (float) obj2->wall->x;
	float y02 = obj2->pos_y + (float) obj2->wall->y;
	
	// absolute x values of most left/right collision pixel:
	float xl1 = x01 + obj1->wall->lx;
	float xr1 = x01 + obj1->wall->rx;
	float xl2 = x02 + obj2->wall->lx;
	float xr2 = x02 + obj2->wall->rx;
	
    bool obj1_before_obj2 = false;
    
    if (obj1->can_move && !obj2->can_move &&
        obj2->wall->pxl != NULL) {      
        
        int32_t x;
        int32_t y;
        
        if (xl1 < xl2) {
            // start beam at most right collision point of obj1:
            x = xr1 - x02;
            y = y01 + obj1->wall->ry_beam - y02;
        } else {
            // start beam at most left collision point of obj1:
            x = xl1 - x02;
            y = y01 + obj1->wall->ly_beam - y02;
        }
        
        obj1_before_obj2 = collisions_beam(obj2, x, y);
        
    } else if (!obj1->can_move && obj2->can_move && 
        obj1->wall->pxl != NULL) {
        
        int32_t x;
        int32_t y;
        
        if (xl2 < xl1) {
            // start beam at most right collision point of obj2:
            x = xr2 - x01;
            y = y02 + obj2->wall->ry_beam - y01;
        } else {
            // start beam at most left collision point of obj2:
            x = xl2 - x01;
            y = y02 + obj2->wall->ly_beam - y01;
        }
        
        obj1_before_obj2 = !collisions_beam(obj1, x, y);
        
    } else {
        // use fast method. calculate points on lines between most left 
        // and most right collision pixel:
        
        // x coordinates on lines:
        
        // use collision pixel in overlapping area:
        float xl = xl1;
        float xr = xr1;
        if (xl1 < xl2) {
            xl = xl2;
        }   
        if (xr1 > xr2) {
            xr = xr2;
        }
        
        // line equation needs x values relative to wall coordinates:
        xl1 = xl - x01;
        xr1 = xr - x01;
        xl2 = xl - x02;
        xr2 = xr - x02;
        
        // y coordinates on lines:
        
        float offset1 = y01 + obj1->wall->offset;
        float offset2 = y02 + obj2->wall->offset;
        
        float yl1 = offset1 + obj1->wall->slope * xl1;
        float yr1 = offset1 + obj1->wall->slope * xr1;
        float yl2 = offset2 + obj2->wall->slope * xl2;
        float yr2 = offset2 + obj2->wall->slope * xr2;
        
        if (yl2 >= yl1 && yr2 >= yr1) {
            obj1_before_obj2 = true;
        } else {
            obj1_before_obj2 = false;
        }
    }
    
	// update render lists:
	if (obj1_before_obj2) {
        obj1->render_before = create_before(
            obj1->render_before, (void*) obj2, 0);
        obj2->render_after = create_before(
            obj2->render_after, (void*) obj1, 0);
	} else {
        obj1->render_after = create_before(
            obj1->render_after, (void*) obj2, 0);
        obj2->render_before = create_before(
            obj2->render_before, (void*) obj1, 0);
	}
}

bool collisions_beam(object_t* obj, int32_t x, int32_t y) {
    
    if (x < 0) {
        x = 0;
    }
    if (y < 0) {
        y = 0;
    }
    
    for (y = y; y < obj->wall->h; y++) {
        
        if (obj->wall->pxl[(y * obj->wall->w_bmp) + x]) {
            
            return(true);
        }
    }
    
    return(false);
}
