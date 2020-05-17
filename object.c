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
    
	
    // video stuff:
    obj_new->render_id = 0;
    obj_new->glcontext = NULL;
    obj_new->window = NULL;
	
	// object list:
	if (obj != NULL) {
		obj->next_object = obj_new;
	}
    
    // render lists:
    obj_new->disable_render = false;
    obj_new->render_before = NULL;
    obj_new->render_after = NULL;
    obj_new->render_blobb = NULL;
    obj_new->render_is_in_blobb = false;
    obj_new->render_early = false;
    
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
	
    // tasks:
    obj_new->tsk = NULL;
    
    // items properties:
    obj_new->itm_props = NULL;
    
    // items:
    obj_new->itm = NULL;
    
    // meters:
    obj_new->mtr = NULL;
    
	// animations:
	obj_new->anim = NULL;
	obj_new->anim_first_call = true;
    obj_new->anim_walk = false;
	
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
    obj_new->disable_collision = false;
	
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
	if (obj->wall != NULL) {
		object_free_walls(obj->wall);
	}
    if (obj->mtr != NULL) {
		object_free_meters(obj->mtr);
	}
	if (obj->anim != NULL) {
		object_free_animations(obj->anim);
	}
	if (obj->txt != NULL) {
		object_free_texts(obj->txt);
	}
    if (obj->txt_language != NULL) {
        free(obj->txt_language);
    }
    if (obj->txt_surface != NULL) {
		SDL_FreeSurface(obj->txt_surface);
	}
    if (obj->render_before != NULL) {
        delete_all(obj->render_before);
    }
    if (obj->render_after != NULL) {
        delete_all(obj->render_after);
    }
    if (obj->render_blobb != NULL) {
        delete_all(obj->render_blobb);
    }
    if (obj->glcontext != NULL) {
        SDL_GL_DeleteContext(obj->glcontext);
    }
    if (obj->tsk != NULL) {
        object_free_tasks(obj->tsk);
    }
    if (obj->itm_props != NULL) {
        object_free_item_props(obj->itm_props);
    }
    if (obj->itm != NULL) {
        object_free_items(obj->itm);
    }
    if (obj->ways != NULL) {
        object_free_waypoints(obj->ways);
    }
	if (obj->col != NULL) {
        object_free_collisions(obj->col);
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

void object_add_meter(
    object_t* obj, uint32_t id, uint8_t type, 
    float scr_pos_x, float scr_pos_y) {
	
    meter_t* mtr = meter_init(type, scr_pos_x, scr_pos_y);
    
    // place at last place in list:
    obj->mtr = create_after(obj->mtr, (void*) mtr, id);
}

void object_free_meters(list_t* mtr) {
	
    mtr = get_first(mtr);
	list_t* mtr_tmp = mtr;
    
	while (mtr != NULL) {
        meter_free((meter_t*) mtr->entry);
		mtr = mtr->next;
	}
    
    delete_all(mtr_tmp);
}

void object_add_animation(object_t* obj, uint32_t id) {
	
    animation_t* anim = animation_init();
    
    // place at first place in list and set as current animation:
    obj->anim = create_before(obj->anim, (void*) anim, id);
}

void object_select_animation(object_t* obj, uint32_t id) {
	
	obj->anim = find_id(obj->anim, id);
}

void object_select_animation_target(object_t* obj, float x, float y) {
    
    float distance_x = x - obj->pos_x;
    float distance_y = y - obj->pos_y;
    
    if (fabsf(distance_x) > fabsf(distance_y)) {
        
        if (distance_x < 0) {
            object_select_animation(obj, ANIMATION_REST_WEST);
        } else {
            object_select_animation(obj, ANIMATION_REST_EAST);
        }
        
    } else {
        
        if (distance_y < 0) {
            object_select_animation(obj, ANIMATION_REST_NORTH);
        } else {
            object_select_animation(obj, ANIMATION_REST_SOUTH);
        }
    }
}

void object_animate(object_t* obj, uint64_t frame) {
	
	// free original surface if it is not part of the animation:
	if (obj->anim_first_call) {
		obj->anim_first_call = 0;
		SDL_FreeSurface(obj->surface);
	}
	
	// get next picture:
	obj->surface = animation_get_next_surface(
        (animation_t*) obj->anim->entry, frame);
}

void object_free_animations(list_t* anim) {
	
	anim = get_first(anim);
	list_t* anim_tmp = anim;
    
	while (anim != NULL) {
        animation_free((animation_t*) anim->entry);
		anim = anim->next;
	}
    
    delete_all(anim_tmp);
}

void object_add_text(object_t* obj, uint32_t id) {
	
    text_t* txt = text_init();
    
    if (txt == NULL) {
        fprintf(stderr, "text_init() failed!\n");
    }
    
    // place before current text in list and set as current text:
    obj->txt = create_before(obj->txt, (void*) txt, id);
    
}

void object_select_text(object_t* obj, uint32_t id) {
    
	obj->txt = find_id(obj->txt, id);
    
}

void object_print_text(object_t* obj) {
	
	obj->txt_print = 1;
	
}

void object_remove_text(object_t* obj, uint32_t id) {
    
    list_t* txt = find_id(obj->txt, id);
    text_free((text_t*) txt->entry);
    obj->txt = delete_single(txt);
    
}

void object_free_texts(list_t* txt) {
	
	txt = get_first(txt);
	list_t* txt_tmp = txt;
    
	while (txt != NULL) {
        text_free((text_t*) txt->entry);
		txt = txt->next;
	}
    
    delete_all(txt_tmp);
}

void object_add_waypoints(object_t* obj, uint32_t id, uint32_t num_ways) {
	
    waypoints_t* ways = waypoints_init(num_ways);
    
    // place at first place in list and set as current waypoints:
    obj->ways = create_before(obj->ways, (void*) ways, id);
}

void object_select_waypoints(object_t* obj, uint32_t id) {
	
	obj->ways = find_id(obj->ways, id);
}

void object_activate_waypoints(object_t* obj) {
	
	uint32_t n;
	
    waypoints_t* ways = (waypoints_t*) obj->ways->entry;
    
	if (ways->pos_are_relative == 1) {
		// if activated the first time
		if (ways->pos_x_relative == NULL) {
			
			// copy positions to relative positions:
			ways->pos_x_relative = malloc(ways->num_ways * sizeof(float));
			ways->pos_y_relative = malloc(ways->num_ways * sizeof(float));
			for (n = 0; n < ways->num_ways; n++) {
				ways->pos_x_relative[n] = ways->pos_x[n];
				ways->pos_y_relative[n] = ways->pos_y[n];
			}
			// convert relative positions to absolute positions in 
			// relation to any object position:
			for (n = 1; n < ways->num_ways; n++) {
				ways->pos_x_relative[n] += ways->pos_x_relative[n-1];
				ways->pos_y_relative[n] += ways->pos_y_relative[n-1];
			}
		}
		// calculate absolute positions:
		for (n = 0; n < ways->num_ways; n++) {
			ways->pos_x[n] = ways->pos_x_relative[n] + obj->pos_x;
			ways->pos_y[n] = ways->pos_y_relative[n] + obj->pos_y;
		}
	} else if (ways->pos_are_relative == -1) {
		// pos are screen positions
		printf("Warning: waypoints for screen positions not implemented!\n");
	}
	
	// activate:
	ways->active = 1;
	
}

void object_get_next_waypoint(object_t* obj) {
	
    waypoints_t* ways = (waypoints_t*) obj->ways->entry;
    
	float pos_x_wp = ways->pos_x[ways->n];
	float pos_y_wp = ways->pos_y[ways->n];
	float border_x = fabsf(obj->vel_x) + 2.0;
	float border_y = fabsf(obj->vel_y) + 2.0;
	
	// object close enough to waypoint?
	if (obj->pos_x > pos_x_wp - border_x && 
		obj->pos_x < pos_x_wp + border_x &&
		obj->pos_y > pos_y_wp - border_y && 
		obj->pos_y < pos_y_wp + border_y ) {
		
        if (ways->frames_wait[ways->n] > ways->frame) {
            ways->frame++;
            return;
        }
        ways->frame = 0;
        
		// select next waypoint:
		ways->n++;
		
		// cycle: TODO: make optional
		if (ways->n == ways->num_ways) {
			ways->n = 0;
		}
		
	}
    
    object_aim_for_waypoint(obj);
}

void object_aim_for_waypoint(object_t* obj) {

    waypoints_t* ways = (waypoints_t*) obj->ways->entry;

	float pos_x_wp = ways->pos_x[ways->n];
	float pos_y_wp = ways->pos_y[ways->n];
	float vel_x_wanted = 0.0;
	float vel_y_wanted = 0.0;
	
	
	vel_x_wanted = pos_x_wp - obj->pos_x;
	vel_y_wanted = pos_y_wp - obj->pos_y;
			
	// normalize:
	float norm = sqrtf(vel_x_wanted * vel_x_wanted + vel_y_wanted * vel_y_wanted);
	vel_x_wanted /= norm;
	vel_y_wanted /= norm;
	vel_x_wanted *= ways->vel_abs[ways->n];
	vel_y_wanted *= ways->vel_abs[ways->n];
	
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

void object_free_waypoints(list_t* ways) {
    
    ways = get_first(ways);
    list_t* ways_tmp = ways;
    
    while (ways != NULL) {
        waypoints_free((waypoints_t*) ways->entry);
        ways = ways->next;
    }
    
    delete_all(ways_tmp);
}

collision_t* object_add_collision(object_t* obj, object_t* partner) {
    
    list_t* col = obj->col;
	collision_t* entry;
    
	// only add if collision does not already exist:
    col = find_id(col, partner->id);
    if (col != NULL) {
        entry = (collision_t*) col->entry;
        // update the old values:
        entry->area_old = entry->area;
        entry->c_x_old = entry->c_x;
        entry->c_y_old = entry->c_y;
        // init area:
        entry->area = 0;
        return(entry);
    }
	
	// add new collision:
	entry = (collision_t*) malloc(sizeof(collision_t));
	
	// add at first place in list:
    obj->col = create_before(obj->col, (void*) entry, partner->id);
	
	// initialize:
    entry->partner = partner;	// add new partner (TODO: Check if stil required)
    entry->area = 0;
    entry->area_old = 0;
    entry->c_x = 0;
    entry->c_y = 0;
    entry->c_x_old = 0;
    entry->c_y_old = 0;
    entry->vel_x = 0;
    entry->vel_y = 0;
    
    return(entry);
}

void object_remove_collision(object_t* obj, object_t* partner) {
	
    list_t* col = obj->col;
    
    if (col == NULL) {
        return;
    }
    
    // find partner in list:
    col = find_id(col, partner->id);
    
    if (col == NULL) {
		return;
    }
    
    // col is now the partner, so remove col:
    free((collision_t*) col->entry);
    obj->col = delete_single(col);
    
}

void object_free_collisions(list_t* col) {
    
    col = get_first(col);
    list_t* col_tmp = col;
    
    while (col != NULL) {
        free((collision_t*) col->entry);
        col = col->next;
    }
    
    delete_all(col_tmp);
}

void object_add_task(object_t* obj, uint32_t id) {
    
    task_t* tsk = (task_t*) malloc(sizeof(task_t));
    tsk->step = 0;
    tsk->task_function = get_task_function(id);
    obj->tsk = create_before(obj->tsk, tsk, id);
}

void object_free_tasks(list_t* lst) {
    
    lst = get_first(lst);
    list_t* tmp = lst;
    
    while (lst != NULL) {
        free((task_t*) lst->entry);
        lst = lst->next;
    }
    
    delete_all(tmp);
}

void object_init_item_props(object_t* obj, SDL_Surface* surf, uint32_t id) {
    
    obj->itm_props = (item_t*) malloc(sizeof(item_t));
    obj->itm_props->surf = surf;
    obj->itm_props->variables = NULL;
    obj->itm_props->step = 0;
    obj->itm_props->item_function = get_item_function(id);
}

void object_free_item_props(item_t* itm_props) {
    
    if (itm_props->surf != NULL) {
        SDL_FreeSurface(itm_props->surf);
    }
    free(itm_props);
}

void object_add_item(object_t* obj, object_t* obj_item, uint32_t id) {
    
    obj_item->disable_collision = true;
    obj_item->disable_render = true;
    obj->itm = create_before(obj->itm, obj_item, id);
}

void object_free_items(list_t* lst) {
    
    delete_all(lst);
}

