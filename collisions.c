#include "collisions.h"

void collisions(object_t* obj, verletbox_t* vbox, float dt) {
	
	bool collision = false;
	uint32_t x, y, x2, y2, x2_min, y2_min, x2_max, y2_max;
	
	verletbox_update(vbox, obj);
	
	// check collisions with background first, 
	// because background is too big for verlet boxes:
	object_t* obj_bg = object_get(obj, OBJECT_BACKGROUND_ID);
	
	obj = object_get_first(obj);
			
	while (obj != NULL) {
				
		if (obj->has_moved && 
            (!obj->disable_collision || !obj->disable_render)) {
		
			obj->vel_lock = false;
			
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
				
				//if (!obj->disable_collision || !obj->disable_render) {
					
					obj->vel_lock = false;	// allow all velocity changes TODO: move up?
					
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
				
				//}
				// get next object obj:
				obj = obj->next_vbox;
			}
			
		}					
	}
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
	
    if (obj1->disable_collision || obj2->disable_collision) {
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
		int32_t x1 = 0;
		int32_t y1 = 0;
		int32_t x2 = 0;
		int32_t y2 = 0;
		int32_t x_min = 0;
		int32_t x_max = 0;
		int32_t y_min = 0;
		int32_t y_max = 0;
		int32_t x2_min = 0;
		int32_t y2_min = 0;
		uint8_t obj1_pxl = 0;
		uint8_t obj2_pxl = 0;
		float norm = 1.0;
	
		// collision directions:
		int32_t x1_dir = 0;
		int32_t y1_dir = 0;
		int32_t x2_dir = 0;
		int32_t y2_dir = 0;
		
		
		// overlapping area of boundary boxes:
		if (x01 < x02) {
			x_min = x02 - x01;
			x2_min = 0;
		} else {
			x_min = 0;
			x2_min = x01 - x02;
		}
		if (y01 < y02) {
			y_min = y02 - y01;
			y2_min = 0;
		} else {
			y_min = 0;
			y2_min = y01 - y02;
		}
		if (xw1 < xw2) {
			x_max = w1;
		} else {
			x_max = w1 - (xw1 - xw2);
		}
		if (yh1 < yh2) {
			y_max = h1;
		} else {
			y_max = h1 - (yh1 - yh2);
		}
		
		for (y1 = y_min; y1 < y_max; y1++) {
			
			for (x1 = x_min; x1 < x_max; x1++) {
				
				if (obj1->wall->pxl != NULL) {
					obj1_pxl = obj1->wall->pxl[(y1 * w1_bmp) + x1];
				} else { // use rectangular boundaries
					obj1_pxl = 1;
				}
				if (obj1_pxl != 0) {
					
					x2 = x2_min + x1 - x_min;
					y2 = y2_min + y1 - y_min;
					if (obj2->wall->pxl != NULL) {
						//fprintf(stderr, "obj2->id: %d\n", obj2->id);
						obj2_pxl = obj2->wall->pxl[(y2 * w2_bmp) + x2];
					} else { // use rectangular boundaries
						obj2_pxl = 1;
					}
					if (obj2_pxl != 0) {
						
						// one pixel is shared by both objects
						
						// so first of all: yes they do collide
						if (collision == false) {
							collision = true;
							col1 = object_add_collision(obj1, obj2);
							col2 = object_add_collision(obj2, obj1);
							col1->area = 0;
							col2->area = 0;
						}
						
						// increase collision area:
						col1->area++;
						col2->area++;
						
						// calculate direction of collision:
						if (x1 >= w1 / 2) {
							if (x1 >= w1 / 2 + w1 % 2) {
								x1_dir++;
							}
						} else {
							x1_dir--;
						}
						if (y1 >= h1 / 2) {
							if (y1 >= h1 / 2 + h1 % 2) {
								y1_dir++;
							}
						} else {
							y1_dir--;
						}
						if (x2 >= w2 / 2) {
							if (x2 >= w2 / 2 + w2 % 2) {
								x2_dir++;
							}
						} else {
							x2_dir--;
						}
						if (y2 >= h2 / 2) {
							if (y2 >= h2 / 2 + h2 % 2) {
								y2_dir++;
							}
						} else {
							y2_dir--;
						}
					}
				}
			}
		}
		
		if (collision) {
			
			// save direction of collision:
			col1->c_x_old = col1->c_x;
			col1->c_y_old = col1->c_y;
			col2->c_x_old = col2->c_x;
			col2->c_y_old = col2->c_y;
			// update direction of collision:
			col1->c_x = x1_dir;
			col1->c_y = y1_dir;
			col2->c_x = x2_dir;
			col2->c_y = y2_dir;
			
			// normalize:
			norm = sqrt(col1->c_x * col1->c_x + col1->c_y * col1->c_y);
			if (norm != 0.0) {
				col1->c_x /= norm;
				col1->c_y /= norm;
			}
			norm = sqrt(col2->c_x * col2->c_x + col2->c_y * col2->c_y);
			if (norm != 0.0) {
				col2->c_x /= norm;
				col2->c_y /= norm;
			}
			
			// if first collision, exchange impulse:
			if (col1->area_old == 0 || col2->area_old == 0) { // TODO: check if area is really updated
				
				// this changes the velocities of the objects:
				collisions_impulse(obj1, obj2, col1, col2);
                
			// if collision area inceases, move away from each other:
			} else {
				if (col1->area >= col1->area_old || 
					col2->area >= col2->area_old) {
					
					float vel_abs1 = sqrtf(obj1->vel_x * obj1->vel_x + obj1->vel_y * obj1->vel_y);
					float vel_abs2 = sqrtf(obj2->vel_x * obj2->vel_x + obj2->vel_y * obj2->vel_y);
					
					if (!obj1->can_move || !obj2->can_move) {
						// move away faster than approached:
						if (obj1->can_move) {
							obj1->pos_x = obj1->pos_x_old;
							obj1->pos_y = obj1->pos_y_old;
                            obj1->vel_x = -vel_abs1 * col1->c_x * 1.1;
							obj1->vel_y = -vel_abs1 * col1->c_y * 1.1;
                            obj1->vel_x -= col1->c_x * 0.5 * (float) rand() / (float) RAND_MAX;
							obj1->vel_y -= col1->c_y * 0.5 * (float) rand() / (float) RAND_MAX;
						}
						if (obj2->can_move) {
							obj2->pos_x = obj2->pos_x_old;
							obj2->pos_y = obj2->pos_y_old;
                            obj2->vel_x = -vel_abs2 * col2->c_x * 1.1;
							obj2->vel_y = -vel_abs2 * col2->c_y * 1.1;
                            obj2->vel_x -= col2->c_x * 0.5 * (float) rand() / (float) RAND_MAX;
							obj2->vel_y -= col2->c_y * 0.5 * (float) rand() / (float) RAND_MAX;
						}
						
					} else {
						// move away:
						if (obj1->can_move) {
							obj1->vel_x -= col1->c_x * 0.2 * dt;
							obj1->vel_y -= col1->c_y * 0.2 * dt;
						}
						if (obj2->can_move) {
							obj2->vel_x -= col2->c_x * 0.2 * dt;
							obj2->vel_y -= col2->c_y * 0.2 * dt;
						}
					}
                    
					// forbid all velocity changes:
					obj1->vel_lock = true;	
					obj2->vel_lock = true;
					
				}
			}
		}
	}
	
	return(collision);
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
	float tmp = 0.0;
	float c1x_tmp = 0.0;
	float c1y_tmp = 0.0;
	float x12 = (float) (obj2->scr_pos_x - obj1->scr_pos_x);
	float y12 = (float) (obj2->scr_pos_y - obj1->scr_pos_y);
	
	if (obj1->can_move) {
		v1x = obj1->vel_x;
		v1y = obj1->vel_y;
	} else {
		c1x = -c2x;
		c1y = -c2y;
	}
	
	if (obj2->can_move) {
		v2x = obj2->vel_x;
		v2y = obj2->vel_y;
	} else {
		c2x = -c1x;
		c2y = -c1y;
	}
	
	// make shure surface vectors point directly toward each other:
	if (obj2->can_move && obj1->can_move) {
		if (c1x != -c2x && c1y != c2y) {
			tmp = -(c2y + c1y) / (c1x + c2x);
			if (tmp != 0.0) {
				c1x_tmp = c1x;
				c1y_tmp = c1y;
				c1x = tmp / sqrtf(1 + tmp * tmp);
				c1y = c1x / tmp;
				if (c1x * c1x_tmp + c1y * c1y_tmp < 0.0) { // > 90 °
					c1x = -c1x;
					c1y = -c1y;
				}
				c2x = -c1x;
				c2y = -c1y;
				
			}
		}
	}	
	
	// velocity in direction of impulse:
	v1n_pre = c1x * v1x + c1y * v1y;
	v2n_pre = c1x * v2x + c1y * v2y;
	
	// impulse:
	v1n = 2.0 * (m1 * v1n_pre + m2 * v2n_pre) / (m1 + m2) - v1n_pre;
	v2n = 2.0 * (m1 * v1n_pre + m2 * v2n_pre) / (m1 + m2) - v2n_pre;
	
	// in 2D an object with smaller velocity can accelerate an object 
	// with higher velocity. To solve this: switch v1n and v2n depending
	// on object positions relative to the hit location:
	if (obj2->can_move && obj1->can_move) {
		if (x12 * c1x + y12 * c1y < 0.0) {
			tmp = v1n;
			v1n = v2n;
			v2n = tmp;
		}
	}
	
	// velocity vertically to impulse direction:
	v1r = c1y * v1x - c1x * v1y;
	v2r = c2y * v2x - c2x * v2y;
	
	if (obj1->can_move) {
		obj1->vel_x = (c1x * v1n + c1y * v1r);
		obj1->vel_y = (c1y * v1n - c1x * v1r);
	}
	
	if (obj2->can_move) {
		obj2->vel_x = (c1x * v2n + c2y * v2r);
		obj2->vel_y = (c1y * v2n - c2x * v2r);
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
