#include "collisions.h"

void collisions(object_t* obj, verletbox_t* vbox) {
	
	short collision = 0;
	int x, y, x2, y2, x2_min, y2_min, x2_max, y2_max;
	
	//Uint32 time;
	//time = SDL_GetTicks();
	
	verletbox_update(vbox, obj);
	//printf("time for verletbox_update: %d\n", SDL_GetTicks() - time);
	//time = SDL_GetTicks();
	
	object_t* obj_bg = object_get(obj, OBJECT_BACKGROUND_ID);
	
	obj = object_get_first(obj);
			
	while (obj != NULL) {
				
		if (obj->can_move) {
		
			obj->vel_lock = 0;
			
			collision = collisions_check(obj_bg, obj);
				
			if (collision == 0) {
				object_remove_collision(obj_bg, obj);
				object_remove_collision(obj, obj_bg);
			}
		
		}
		
		obj = obj->next_object;
	}
	
	
	//printf("time for background collisions: %d\n", SDL_GetTicks() - time);
	//time = SDL_GetTicks();
	
	object_t* obj_b = NULL;
	
	for (x = 0; x < vbox->num_w; x++) {
		for (y = 0; y < vbox->num_h; y++) {
			
			obj = verletbox_get_first_object(vbox->boxes[x][y]);
			
			while (obj != NULL) {
				
				if (obj->disable_collision == 0) {
					
					obj->vel_lock = 0;	// allow all velocity changes TODO: move up?
					
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
							} else {                    // neighbor vbox
								obj_b = verletbox_get_first_object(vbox->boxes[x2][y2]);
							}
						
							while (obj_b != NULL) {
						
								if (obj->can_move || obj_b->can_move) {
									collision = collisions_check(obj, obj_b);
					
									if (collision == 0) {
										object_remove_collision(obj, obj_b);
										object_remove_collision(obj_b, obj);
									}
								}

								// get next object obj_b:
								obj_b = obj_b->next_vbox;
							}

						}
					}
				
				}
				// get next object obj:
				obj = obj->next_vbox;
			}
			
		}					
	}
	//printf("time for other collisions: %d\n", SDL_GetTicks() - time);
	
}

short collisions_check(object_t* obj1, object_t* obj2) {
	
	int collision = 0;
	
	// boundary boxes base positions:
	int x01 = (int) obj1->pos_x;
	int y01 = (int) obj1->pos_y;
	int x02 = (int) obj2->pos_x;
	int y02 = (int) obj2->pos_y;
	
	// boundary boxes widths and heights:
	int w1 = obj1->surface->w;
	int h1 = obj1->surface->h;
	int w2 = obj2->surface->w;
	int h2 = obj2->surface->h;
	
	int xw1 = x01 + obj1->surface->w;
	int yh1 = y01 + obj1->surface->h;
	int xw2 = x02 + obj2->surface->w;
	int yh2 = y02 + obj2->surface->h;
	
	// check collision of surfaces first:
	
	if (obj2->disable_collision == 0) { // is this the right place?
		
		if (obj1->id == OBJECT_BACKGROUND_ID) {
			
			// check for collision of background pixels 
			// with corners of boundary box:
			
			Uint8* pxl = obj1->wall->pxl;
			unsigned int w = obj1->wall->w_bmp;
			
			if        (pxl[(y02 * w) + x02] != 0) {
				collision = 1;
			} else if (pxl[(y02 * w) + xw2] != 0) {
				collision = 1;
			} else if (pxl[(yh2 * w) + x02] != 0) {
				collision = 1;
			} else if (pxl[(yh2 * w) + xw2] != 0) {
				collision = 1;
			}
			
		} else if (obj2->id == OBJECT_BACKGROUND_ID) {
			
			// check for collision of background pixels 
			// with boundary box:
			
			Uint8* pxl = obj2->wall->pxl;
			unsigned int w = obj2->wall->w_bmp;
			
			if        (pxl[(y01 * w) + x01] != 0) {
				collision = 1;
			} else if (pxl[(y01 * w) + xw1] != 0) {
				collision = 1;
			} else if (pxl[(yh1 * w) + x01] != 0) {
				collision = 1;
			} else if (pxl[(yh1 * w) + xw1] != 0) {
				collision = 1;
			}
			
		} else {
			
			// check for collision of boundary boxes:
			if (xw2 > x01 && x02 < xw1 && 
				yh2 > y01 && y02 < yh1) {
				
				collision = 1;
				
			}
		}
	
	}
		
	
	if (collision &&
		obj1->id != OBJECT_SURFDISPLAY_ID &&
		obj1->id != OBJECT_BACKGROUND_ID &&
		obj2->id != OBJECT_SURFDISPLAY_ID &&
		obj2->id != OBJECT_BACKGROUND_ID) {
		
		collisions_update_render(obj1, obj2);
	}
	
	// check for pixel wise collision:
	if (collision == 1) {
		
		collision = 0;
		
		collision_t* col1 = NULL;
		collision_t* col2 = NULL;
		unsigned int w1_bmp = obj1->wall->w_bmp;
		unsigned int w2_bmp = obj2->wall->w_bmp;
		
		x01 = (int) obj1->pos_x + obj1->wall->x;
		y01 = (int) obj1->pos_y + obj1->wall->y;
		x02 = (int) obj2->pos_x + obj2->wall->x;
		y02 = (int) obj2->pos_y + obj2->wall->y;

		w1 = obj1->wall->w;
		h1 = obj1->wall->h;
		w2 = obj2->wall->w;
		h2 = obj2->wall->h;
		
		xw1 = x01 + w1;
		yh1 = y01 + h1;
		xw2 = x02 + w2;
		yh2 = y02 + h2;
		
		// current position in overlapping area:
		int x1 = 0;
		int y1 = 0;
		int x2 = 0;
		int y2 = 0;
		int x_min = 0;
		int x_max = 0;
		int y_min = 0;
		int y_max = 0;
		int x2_min = 0;
		int y2_min = 0;
		Uint8 obj1_pxl = 0;
		Uint8 obj2_pxl = 0;
		float norm = 1.0;
	
		// collision directions:
		int x1_dir = 0;
		int y1_dir = 0;
		int x2_dir = 0;
		int y2_dir = 0;
		
		
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
		
		// debug:
		/*if (obj1->id == OBJECT_HERO_ID || obj2->id == OBJECT_HERO_ID) {
			fprintf(stderr, "\n\n\n");
			fprintf(stderr, "obj1->id = %d, obj2->id = %d\n", 
				obj1->id, obj2->id);
			fprintf(stderr, "obj1->scr_pos_x = %f, obj2->scr_pos_x = %f\n", 
				obj1->scr_pos_x, obj2->scr_pos_x);
			fprintf(stderr, "obj1->surface->w = %d, obj2->surface->w = %d\n", 
				obj1->surface->w, obj2->surface->w);
			fprintf(stderr, "obj1->scr_pos_y = %f, obj2->scr_pos_y = %f\n", 
				obj1->scr_pos_y, obj2->scr_pos_y);
			fprintf(stderr, "obj1->surface->h = %d, obj2->surface->h = %d\n", 
				obj1->surface->h, obj2->surface->h);
			fprintf(stderr, "x_min = %d, x_max = %d, x2_min = %d\n", 
				x_min, x_max, x2_min);
			fprintf(stderr, "y_min = %d, y_max = %d, y2_min = %d\n", 
				y_min, y_max, y2_min);
		}*/
		
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
						if (collision == 0) {
							collision = 1;
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
								//x1_dir += x1 - w1 / 2;
								x1_dir++;
							}
						} else {
							//x1_dir -= w1 / 2 - x1;
							x1_dir--;
						}
						if (y1 >= h1 / 2) {
							if (y1 >= h1 / 2 + h1 % 2) {
								//y1_dir += y1 - h1 / 2;
								y1_dir++;
							}
						} else {
							//y1_dir -= h1 / 2 - y1;
							y1_dir--;
						}
						if (x2 >= w2 / 2) {
							if (x2 >= w2 / 2 + w2 % 2) {
								//x2_dir += x2 - w2 / 2;
								x2_dir++;
							}
						} else {
							//x2_dir -= w2 / 2 - x2;
							x2_dir--;
						}
						if (y2 >= h2 / 2) {
							if (y2 >= h2 / 2 + h2 % 2) {
								//y2_dir += y2 - h2 / 2;
								y2_dir++;
							}
						} else {
							//y2_dir -= h2 / 2 - y2;
							y2_dir--;
						}
					} else {
						/*if (obj2->id == OBJECT_HERO_ID) {
							fprintf(stderr, "o");
						}*/
					}
				} else {
					/*if (obj2->id == OBJECT_HERO_ID) {
						fprintf(stderr, "0");
					}*/
				}
			}
			/*if (obj2->id == OBJECT_HERO_ID) {
				fprintf(stderr, "\n");
			}*/
		}
		
		if (collision) {
			
			/*printf("\n");
			printf("x01: %d, x02: %d\n", x01, x02);
			printf("y01: %d, y02: %d\n", y01, y02);
			printf("x_max: %d\n", x_max);
			printf("y_max: %d\n", y_max);
			printf("\n");
			printf("x_min: %d, x2_min: %d\n", x_min, x2_min);
			printf("y_min: %d, y2_min: %d\n", y_min, y2_min);
			printf("x_max: %d, x2: %d\n", x2);
			printf("y_max: %d, y2: %d\n", y2);
			printf("\n");
			printf("x1_dir: %d, x2_dir: %d\n", x1_dir, x2_dir);
			printf("y1_dir: %d, y2_dir: %d\n", y1_dir, y2_dir);
			printf("\n");*/
			
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
							obj1->vel_x = -vel_abs1 * col1->c_x * 1.1 - col1->c_x * 0.3;
							obj1->vel_y = -vel_abs1 * col1->c_y * 1.1 - col1->c_y * 0.3;
						}
						if (obj2->can_move) {
							obj2->pos_x = obj2->pos_x_old;
							obj2->pos_y = obj2->pos_y_old;
							obj2->vel_x = -vel_abs2 * col2->c_x * 1.1 - col1->c_x * 0.3;
							obj2->vel_y = -vel_abs2 * col2->c_y * 1.1 - col1->c_y * 0.3;
						}
						
					} else {
						// move away:
						if (obj1->can_move) {
							obj1->vel_x -= col1->c_x * 0.2;
							obj1->vel_y -= col1->c_y * 0.2;
						}
						if (obj2->can_move) {
							obj2->vel_x -= col2->c_x * 0.2;
							obj2->vel_y -= col2->c_y * 0.2;
						}
					}
					
					/*if (obj1->id == OBJECT_HERO_ID || obj2->id == OBJECT_HERO_ID) {
						printf("\ncollision area increase\n");
						printf("obj1->id: %d, obj2->id: %d\n", obj1->id, obj2->id);
						printf("col1->c_x: %f, col2->c_x: %f\n", col1->c_x, col2->c_x);
						printf("col1->c_y: %f, col2->c_y: %f\n", col1->c_y, col2->c_y);
						printf("obj1->vel_x: %f, obj2->vel_x: %f\n", obj1->vel_x, obj2->vel_x);
						printf("obj1->vel_y: %f, obj2->vel_y: %f\n\n", obj1->vel_y, obj2->vel_y);
					}*/
					
					// forbid all velocity changes:
					obj1->vel_lock = 1;	
					obj2->vel_lock = 1;
					
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
	
	//if (obj1->id != OBJECT_BACKGROUND_ID) {
	if (obj1->can_move) {
		v1x = obj1->vel_x;
		v1y = obj1->vel_y;
	} else {
		c1x = -c2x;
		c1y = -c2y;
	}
	//if (obj2->id != OBJECT_BACKGROUND_ID) {
	if (obj2->can_move) {
		v2x = obj2->vel_x;
		v2y = obj2->vel_y;
	} else {
		c2x = -c1x;
		c2y = -c1y;
	}
	
	// make shure surface vectors point directly toward each other:
	//if (obj2->id != OBJECT_BACKGROUND_ID && obj1->id != OBJECT_BACKGROUND_ID) {
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
		obj1->vel_x = c1x * v1n + c1y * v1r;
		obj1->vel_y = c1y * v1n - c1x * v1r;
	}
	
	if (obj2->can_move) {
		obj2->vel_x = c1x * v2n + c2y * v2r;
		obj2->vel_y = c1y * v2n - c2x * v2r;
	}
	
	// for debuging:
	/*
	fprintf(stderr, "\nobj1->id = %d\n", obj1->id);
	fprintf(stderr, "scr_pos_x = %d, c1x = %e, v1x = %e, vel_x = %e, acc_x = %e\n", obj1->scr_pos_x, c1x, v1x, obj1->vel_x, obj1->acc_x);
	fprintf(stderr, "scr_pos_y = %d, c1y = %e, v1y = %e, vel_y = %e, acc_y = %e\n", obj1->scr_pos_y, c1y, v1y, obj1->vel_y, obj1->acc_y);
	fprintf(stderr, "m1 = %e, v1n_pre = %e, v1r = %e, v1n = %e\n", m1, v1n_pre, v1r, v1n);
	fprintf(stderr, "collisions:\n");
	collision_t* col;
	col = obj1->col;
	while (col != NULL) {
		fprintf(stderr, "%d, ", col->partner->id);
		col = col->next;
	}
	fprintf(stderr, "\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\nobj2->id = %d\n", obj2->id);
	fprintf(stderr, "scr_pos_x = %d, c2x = %e, v2x = %e, vel_x = %e, acc_x = %e\n", obj2->scr_pos_x, c2x, v2x, obj2->vel_x, obj2->acc_x);
	fprintf(stderr, "scr_pos_y = %d, c2y = %e, v2y = %e, vel_y = %e, acc_y = %e\n", obj2->scr_pos_y, c2y, v2y, obj2->vel_y, obj2->acc_x);
	fprintf(stderr, "m2 = %e, v2n_pre = %e, v2r = %e, v2n = %e\n", m2, v2n_pre, v2r, v2n);
	fprintf(stderr, "collisions:\n");
	col = obj2->col;
	while (col != NULL) {
		fprintf(stderr, "%d, ", col->partner->id);
		col = col->next;
	}
	fprintf(stderr, "\n");
	*/
}

void collisions_update_render(object_t* obj1, object_t* obj2) {
	
	// calculate points on lines between most left and most right 
	// collision pixel:
	
	float x01 = obj1->pos_x + (float) obj1->wall->x;
	float y01 = obj1->pos_y + (float) obj1->wall->y;
	float x02 = obj2->pos_x + (float) obj2->wall->x;
	float y02 = obj2->pos_y + (float) obj2->wall->y;
	
	float offset1 = y01 + obj1->wall->offset;
	float offset2 = y02 + obj2->wall->offset;
	
	// x coordinates on lines:
	
	// absolute x values of most left/right collision pixel:
	float xl1 = x01 + obj1->wall->lx;
	float xr1 = x01 + obj1->wall->rx;
	float xl2 = x02 + obj2->wall->lx;
	float xr2 = x02 + obj2->wall->rx;
	
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
	float yl1 = offset1 + obj1->wall->slope * xl1;
	float yr1 = offset1 + obj1->wall->slope * xr1;
	float yl2 = offset2 + obj2->wall->slope * xl2;
	float yr2 = offset2 + obj2->wall->slope * xr2;
	
	// update render list:
	if (yl2 >= yl1 && yr2 >= yr1) {
		// render obj2 right after obj1:
		if (obj1->next_render != NULL) {
			obj1->next_render->prev_render = obj2;
		}
		if (obj2->next_render != NULL) {
			obj2->next_render->prev_render = obj2->prev_render;
		}
		if (obj2->prev_render != NULL) {
			obj2->prev_render->next_render = obj2->next_render;
		}
		if (obj1->next_render != NULL) {
			if (obj1->next_render != obj2) {
				obj2->next_render = obj1->next_render;
			} else {
				obj2->next_render = obj1->next_render->next_render;
			}
		} else {
			obj2->next_render = NULL;
		}
		obj1->next_render = obj2;
		obj2->prev_render = obj1;
	} else {
		// render obj2 right before obj1:
		if (obj1->prev_render != NULL) {
			obj1->prev_render->next_render = obj2;
		}
		if (obj2->prev_render != NULL) {
			obj2->prev_render->next_render = obj2->next_render;
		}
		if (obj2->next_render != NULL) {
			obj2->next_render->prev_render = obj2->prev_render;
		}
		if (obj1->prev_render != NULL) {
			if (obj1->prev_render != obj2) {
				obj2->prev_render = obj1->prev_render;
			} else {
				obj2->prev_render = obj1->prev_render->prev_render;
			}
		} else {
			obj2->prev_render = NULL;
		}
		obj1->prev_render = obj2;
		obj2->next_render = obj1;
	}
}
