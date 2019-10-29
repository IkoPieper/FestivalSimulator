#include "object.h"

object_t* object_get_last(object_t* obj) {
	
	if (obj != NULL) {
		while(obj->next_object != NULL) {
			obj = obj->next_object;
		}
	}
	
	return(obj);
}

object_t* object_get_first(object_t* obj) {
	
	if (obj != NULL) {
		while(obj->prev_object != NULL) {
			obj = obj->prev_object;
		}
	}
	
	return(obj);
}

object_t* object_add(object_t* obj, uint32_t id) {
	
	object_t* obj_new = (object_t*) malloc(sizeof(object_t));
	
	// object id:
	obj_new->id = id;
	
	// object list:
	obj = object_get_last(obj);
	obj_new->prev_object = obj;
	obj_new->next_object = NULL;
    
	obj_new->render_id = 0;
	
	// object list:
	if (obj != NULL) {
		obj->next_object = obj_new;
	}
    
    // render lists:
    obj_new->render_before = NULL;
    obj_new->render_after = NULL;
    obj_new->render_blobb = NULL;
    obj_new->render_is_in_blobb = false;
    
	// verlet boxes:
	obj_new->prev_vbox = NULL;
	obj_new->next_vbox = NULL;
	obj_new->vbox_x = 0;
	obj_new->vbox_y = 0;
	
	// physics:
	obj_new->can_move = 0;
	obj_new->has_moved = 1;	// init with 1 to set up render list
	obj_new->mass = 1.0;
	obj_new->damping = 0.2;
	obj_new->pos_x = 1.0;
	obj_new->pos_x = 0.0;
	obj_new->pos_y = 0.0;
	obj_new->pos_x_old = 0.0;
	obj_new->pos_y_old = 0.0;
	obj_new->vel_x = 0.0;
	obj_new->vel_y = 0.0;
	obj_new->vel_lock = 0;
	obj_new->acc_x = 0.0;
	obj_new->acc_y = 0.0;
	obj_new->acc_abs = 0.01;
	
	// screen positions:
	obj_new->scr_pos_x = 0.0;
	obj_new->scr_pos_y = 0.0;
	obj_new->min_scr_pos_x = -99999.0;
	obj_new->max_scr_pos_x = 99999.0;
	obj_new->min_scr_pos_y = -99999.0;
	obj_new->max_scr_pos_y = 99999.0;
	
	// bitmaps:
	obj_new->surface = NULL;
	obj_new->wall = NULL;
	
	// animations:
	obj_new->anim = NULL;
	obj_new->anim_first_call = 1;
	
	// texts:
	obj_new->txt_language = (char *) malloc(3 * sizeof(char));
	obj_new->txt_language[0] = 'd';
	obj_new->txt_language[1] = 'e';
	obj_new->txt_language[2] = '\0';
	obj_new->txt = NULL;
	obj_new->txt_print = 0;
	obj_new->txt_surface = NULL;

	// waypoints:
	obj_new->ways = NULL;
	
	// collisions:
	obj_new->col = NULL;
    obj_new->disable_collision = 0;
	
	return(obj_new);
}

walls_t* object_init_walls(SDL_Surface* surf_wall, SDL_Surface* surf) {
	walls_t* wall = (walls_t*) malloc(sizeof(walls_t));
	if (surf_wall != NULL) {
		wall->pxl = (uint8_t*) surf_wall->pixels;
		wall->x = 0;
		wall->y = surf->h - surf_wall->h;
		
		wall->w = surf_wall->w;
		// bitmaps are stored as 32 bit blocks in memory. as we use
		// 8 bit per pixel, we have to account for additional junk
		// pixels that might be stored at the end of every row:
		if (surf_wall->w % 4 == 0) {
			wall->w_bmp = surf_wall->w;
		} else {
			wall->w_bmp = surf_wall->w + (4 - (surf_wall->w % 4));
		}
		wall->h = surf_wall->h;
		// find most left pixel:
		bool pixel_found = 0;
		for (wall->lx = 0; wall->lx < wall->w_bmp; wall->lx++) {
			for (wall->ly = wall->h - 1; wall->ly > 0; wall->ly--) {
				if (wall->pxl[(wall->ly * wall->w_bmp) + wall->lx] != 0) {
					pixel_found = 1;
					break;
				}
			}
			if (pixel_found) {
				break;
			}
		}
		// find most right pixel:
		pixel_found = 0;
		for (wall->rx = wall->w_bmp - 1; wall->rx >= 0; wall->rx--) {
			for (wall->ry = wall->h - 1; wall->ry > 0; wall->ry--) {
				if (wall->pxl[(wall->ry * wall->w_bmp) + wall->rx] != 0) {
					pixel_found = 1;
					break;
				}
			}
			if (pixel_found) {
				break;
			}
		}
		wall->slope = 
			((float) wall->ry - (float) wall->ly) / 
			((float) wall->rx - (float) wall->lx);
		
		wall->offset = 
			(float) wall->ly - 
			(float) wall->lx * wall->slope;
			
	} else {
		wall->x = 0;
		wall->y = 0;
		wall->w = surf->w;
		if (surf->w % 4 == 0) {
			wall->w_bmp = surf->w;
		} else {
			wall->w_bmp = surf->w + (4 - (surf->w % 4));
		}
		wall->h = surf->h;
		wall->rx = surf->w;
		wall->ry = surf->h;
		wall->lx = 0;
		wall->ly = surf->h;
		wall->slope = 0;
		wall->offset = surf->h;
	}
	
	wall->surf = surf_wall;
	
	return(wall);
}

void object_free_walls(walls_t* wall) {
	SDL_FreeSurface(wall->surf);
	free(wall);
}

object_t* object_remove(object_t* obj, uint32_t id) {
	
	obj = object_get_last(obj);
	
	object_t* ret;
	
	while (obj->id != id) {
		obj = obj->prev_object;
	}
	if (obj->prev_object != NULL) {
		obj->prev_object->next_object = obj->next_object;
	}
	if (obj->next_object != NULL) {
		obj->next_object->prev_object = obj->prev_object;
		ret = obj->next_object;
	} else {
		ret = obj->prev_object;
	}
	
	// TODO: clean other things (animations, movements, ...)
	
	if (obj->surface != NULL && obj->anim_first_call) {
		SDL_FreeSurface(obj->surface);
	}
	if (obj->txt_surface != NULL) {
		SDL_FreeSurface(obj->txt_surface);
	}
	if (obj->wall != NULL) {
		object_free_walls(obj->wall);
	}
	if (obj->anim != NULL) {
		object_free_animations(obj->anim);
	}
    if (obj->txt_language != NULL) {
        free(obj->txt_language);
    }
	if (obj->txt != NULL) {
		object_free_texts(obj->txt);
	}
    if (obj->render_before != NULL) {
        obj->render_before = listobj_free(obj->render_before);
    }
    if (obj->render_after != NULL) {
        obj->render_after = listobj_free(obj->render_after);
    }
    if (obj->render_blobb != NULL) {
        obj->render_blobb = listobj_free(obj->render_blobb);
    }
	
	free(obj);
	
	return(ret);
}

void object_clean_up(object_t* obj) {
	obj = object_get_last(obj);
	while (obj != NULL) {
		obj = object_remove(obj, obj->id);
	}
}

object_t* object_get(object_t* obj, uint32_t id) {
	
	obj = object_get_last(obj);
	while (obj->id != id) {
		obj = obj->prev_object;
	}
	
	return(obj);
}

uint32_t object_get_count(object_t* obj) {
	
	obj = object_get_last(obj);
	uint32_t count = 0;
	while (obj != NULL) {
		count++;
		obj = obj->prev_object;
	}
	
	return(count);
}

void object_add_animation(object_t* obj, uint32_t id) {
	
	animation_t* anim = animation_init(id);
	
	// place at first place in list:
	anim->next = obj->anim;
	
	if (obj->anim != NULL) {
		obj->anim->prev = anim;
	}
	
	// set as current animation:
	obj->anim = anim;
}

void object_select_animation(object_t* obj, uint32_t id) {
	
	animation_t* anim = obj->anim;
	
	// get first animation:
	while (anim->prev != NULL) {
		anim = anim->prev;
	}
	// find animation:
	while (anim->id != id) {
		anim = anim->next;
	}
	// select animation:
	obj->anim = anim;
}

void object_animate(object_t* obj, uint64_t frame) {
	
	// free original surface if it is not part of the animation:
	if (obj->anim_first_call) {
		obj->anim_first_call = 0;
		SDL_FreeSurface(obj->surface);
	}
	
	// get next picture:
	obj->surface = animation_get_next_surface(obj->anim, frame);
}

/*void object_remove_animation(object_t* obj, uint32_t id) {
	
	if (obj->anim->id == id) {
		object_remove_selected_animation(obj);
		return;
	}
	
	animation_t* anim = obj->anim;
	
	// get first animation:
	while (anim->prev != NULL) {
		anim = anim->prev;
	}
	// find animation:
	while (anim->id != id) {
		anim = anim->next;
	}
	// update animation list:
	if (anim->prev != NULL) {
		anim->prev = anim->next;
	}
	if (anim->next != NULL) {
		anim->next->prev = anim->prev;
	}
	
	animation_free(anim);
}*/

/*void object_remove_selected_animation(object_t* obj) {
	
	animation_t* anim = obj->anim;
	
	// update animation list:
	if (anim->prev != NULL) {
		anim->prev = anim->next;
	}
	if (anim->next != NULL) {
		anim->next->prev = anim->prev;
	}
	// choose new selected animation:
	if (anim->prev != NULL) {
		obj->anim = anim->prev;
	} else if (anim->next != NULL) {
		obj->anim = anim->next;
	} else {
		obj->anim = NULL;
	}
	
	animation_free(anim);
}*/

void object_free_animations(animation_t* anim) {
	
	animation_t* anim_tmp;
	
	// get first animation:
	while (anim->prev != NULL) {
		anim = anim->prev;
	}
	
	while (anim != NULL) {
		anim_tmp = anim;
		anim = anim->next;
		animation_free(anim_tmp);
	}
	
}

void object_add_text(object_t* obj, uint32_t id) {
	
	text_t* txt = text_init(id);
	
	// place at first place in list:
	txt->next = obj->txt;
	
	if (obj->txt != NULL) {
		obj->txt->prev = txt;
	}
	
	// set as current text:
	obj->txt = txt;
}

void object_select_text(object_t* obj, uint32_t id) {
	
	text_t* txt = obj->txt;
	
	// get first text:
	while (txt->prev != NULL) {
		txt = txt->prev;
	}
	// find text:
	while (txt->id != id) {
		txt = txt->next;
	}
	// select text:
	obj->txt = txt;
}

void object_print_text(object_t* obj) {
	
	obj->txt_print = 1;
	
}

void object_free_texts(text_t* text) {
	
	text_t* text_tmp;
	
	// get first text:
	while (text->prev != NULL) {
		text = text->prev;
	}
	
	while (text != NULL) {
		text_tmp = text;
		text = text->next;
		text_free(text_tmp);
	}
	
}

void object_add_waypoints(object_t* obj, uint32_t id, uint32_t num_ways) {
	
	waypoints_t* ways = waypoints_init(id, num_ways);
	
	// place at first place in list:
	ways->next = obj->ways;
	
	if (obj->ways != NULL) {
		obj->ways->prev = ways;
	}
	
	// set as current waypoints:
	obj->ways = ways;
	
}

void object_select_waypoints(object_t* obj, uint32_t id) {
	
	waypoints_t* ways = obj->ways;
	
	// get first waypoints:
	while (ways->prev != NULL) {
		ways = ways->prev;
	}
	// find waypoints:
	while (ways->id != id) {
		ways = ways->next;
	}
	// select waypoints:
	obj->ways = ways;
}

void object_remove_waypoints(object_t* obj, uint32_t id) {
	
	if (obj->ways->id == id) {
		object_remove_selected_waypoints(obj);
		return;
	}
	
	waypoints_t* ways = obj->ways;
	
	// get first waypoints:
	while (ways->prev != NULL) {
		ways = ways->prev;
	}
	// find waypoints:
	while (ways->id != id) {
		ways = ways->next;
	}
	// update waypoints list:
	if (ways->prev != NULL) {
		ways->prev = ways->next;
	}
	if (ways->next != NULL) {
		ways->next->prev = ways->prev;
	}
	
	waypoints_free(ways);
}

void object_remove_selected_waypoints(object_t* obj) {
	
	waypoints_t* ways = obj->ways;
	
	// update waypoints list:
	if (ways->prev != NULL) {
		ways->prev = ways->next;
	}
	if (ways->next != NULL) {
		ways->next->prev = ways->prev;
	}
	// choose new selected waypoints:
	if (ways->prev != NULL) {
		obj->ways = ways->prev;
	} else if (ways->next != NULL) {
		obj->ways = ways->next;
	} else {
		obj->ways = NULL;
	}
	
	waypoints_free(ways);
}

void object_activate_waypoints(object_t* obj) {
	
	uint32_t n;
	
	if (obj->ways->pos_are_relative == 1) {
		// if activated the first time
		if (obj->ways->pos_x_relative == NULL) {
			
			// copy positions to relative positions:
			obj->ways->pos_x_relative = malloc(obj->ways->num_ways * sizeof(float));
			obj->ways->pos_y_relative = malloc(obj->ways->num_ways * sizeof(float));
			for (n = 0; n < obj->ways->num_ways; n++) {
				obj->ways->pos_x_relative[n] = obj->ways->pos_x[n];
				obj->ways->pos_y_relative[n] = obj->ways->pos_y[n];
			}
			// convert relative positions to absolute positions in 
			// relation to any object position:
			for (n = 1; n < obj->ways->num_ways; n++) {
				obj->ways->pos_x_relative[n] += obj->ways->pos_x_relative[n-1];
				obj->ways->pos_y_relative[n] += obj->ways->pos_y_relative[n-1];
			}
		}
		// calculate absolute positions:
		for (n = 0; n < obj->ways->num_ways; n++) {
			obj->ways->pos_x[n] = obj->ways->pos_x_relative[n] + obj->pos_x;
			obj->ways->pos_y[n] = obj->ways->pos_y_relative[n] + obj->pos_y;
		}
	} else if (obj->ways->pos_are_relative == -1) {
		// pos are screen positions
		printf("Warning: waypoints for screen positions not implemented!\n");
	}
	
	// activate:
	obj->ways->active = 1;
	
}

void object_get_next_waypoint(object_t* obj) {
	
	float pos_x_wp = obj->ways->pos_x[obj->ways->n];
	float pos_y_wp = obj->ways->pos_y[obj->ways->n];
	float border_x = fabsf(obj->vel_x) + 2.0;
	float border_y = fabsf(obj->vel_y) + 2.0;
	
	// object close enough to waypoint?
	if (obj->pos_x > pos_x_wp - border_x && 
		obj->pos_x < pos_x_wp + border_x &&
		obj->pos_y > pos_y_wp - border_y && 
		obj->pos_y < pos_y_wp + border_y ) {
		
		// select next waypoint:
		obj->ways->n++;
		
		// cycle: TODO: make optional
		if (obj->ways->n == obj->ways->num_ways) {
			obj->ways->n = 0;
		}
		
	}
}

void object_aim_for_waypoint(object_t* obj) {

	float pos_x_wp = obj->ways->pos_x[obj->ways->n];
	float pos_y_wp = obj->ways->pos_y[obj->ways->n];
	float vel_x_wanted = 0.0;
	float vel_y_wanted = 0.0;
	
	
	vel_x_wanted = pos_x_wp - obj->pos_x;
	vel_y_wanted = pos_y_wp - obj->pos_y;
			
	// normalize:
	float norm = sqrtf(vel_x_wanted * vel_x_wanted + vel_y_wanted * vel_y_wanted);
	vel_x_wanted /= norm;
	vel_y_wanted /= norm;
	vel_x_wanted *= obj->ways->vel_abs[obj->ways->n];
	vel_y_wanted *= obj->ways->vel_abs[obj->ways->n];
	
	// modify current velocity a step towards the velocity wanted:
	if (obj->vel_x < vel_x_wanted - 0.2) {
		obj->vel_x += 0.2;
	} else if (obj->vel_x > vel_x_wanted + 0.2) {
		obj->vel_x -= 0.2;
	}
	if (obj->vel_y < vel_y_wanted - 0.2) {
		obj->vel_y += 0.2;
	} else if (obj->vel_y > vel_y_wanted + 0.2) {
		obj->vel_y -= 0.2;
	}
	
}


collision_t* object_add_collision(object_t* obj, object_t* partner) {
	
	collision_t* col = NULL;
	
	// only add if collision does not already exist:
	col = obj->col;
	while (col != NULL) {
		if (col->partner == partner) { // partner found
			// update the old values:
			col->area_old = col->area;
			col->c_x_old = col->c_x;
			col->c_y_old = col->c_y;
			// init area:
			col->area = 0;
			return(col);
		}
		col = col->next;
	}
	
	// add new collision:
	col = (collision_t*) malloc(sizeof(collision_t));
	
	// add at first place in list:
	if (obj->col != NULL) {
		obj->col->prev = col;
		col->next = obj->col;
	} else {
		col->next = NULL;
	}
	obj->col = col;
	col->prev = NULL;
	
	// initialize:
    col->partner = partner;	// add new partner
    col->area = 0;
    col->area_old = 0;
    col->c_x = 0;
    col->c_y = 0;
    col->c_x_old = 0;
    col->c_y_old = 0;
    col->vel_x = 0;
    col->vel_y = 0;
    
    return(col);
}

void object_remove_collision(object_t* obj, object_t* partner) {
	
	collision_t* col = obj->col;
	
	if (col == NULL) {
		return;
	}
	
	// go to first collision:
	while (col->prev != NULL) {
		col = col->prev;
	}
	
	// find partner in list:
	while (col->partner != partner) {
		col = col->next;
		if (col == NULL) {
			return;
		}
	}
	
	// col is now the partner, so remove col:
	if (col->prev != NULL) {
		col->prev->next = col->next;
	}
	if (col->next != NULL) {
		col->next->prev = col->prev;
	}
	if (col->prev == NULL && col->next == NULL) { // if the only one
		obj->col = NULL;
	} else if (col->prev == NULL) {
		obj->col = col->next;
	}
	
	free(col);
	
}

listobj_t* listobj_add(listobj_t* first, object_t* obj) {
	
	listobj_t* entry = (listobj_t*) malloc(sizeof(listobj_t));
	entry->obj = obj;
	entry->next = first;
	
	return(entry);  // new first element
	
}

listobj_t* listobj_remove(listobj_t* first, listobj_t* entry) {
	
	listobj_t* tmp = first;
	
	if (tmp == entry) {
		tmp = entry->next;
		free(entry);
		return(tmp);  // new first element
	}
	
	while (tmp->next != entry) {
		tmp = tmp->next;
	}
	tmp->next = entry->next;
	free(entry);
	return(first);
	
}

bool listobj_is_member(listobj_t* first, object_t* obj) {
    
    while (first != NULL) {
        
        if (first->obj == obj) {
            return(true);
        }
        
        first = first->next;
    }
    return(false);
}
uint32_t listobj_count_objects(listobj_t* first) {
    
    uint32_t count = 0;
    
    while (first != NULL) {
        
        count++;
        
        first = first->next;
    }
    return(count);
    
}

listobj_t* listobj_free(listobj_t* first) {
    while (first != NULL) {
        listobj_t* tmp = first;
        first = first->next;
        free(tmp);
    }
    return(NULL);
}
