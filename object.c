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
    obj_new->in_vbox = false;
	
	// physics:
	obj_new->can_move = false;
	obj_new->has_moved = true;	// init with true to set up render list
	obj_new->mass = 1.0;
	obj_new->damping = 0.2;
    obj_new->disable_damping = false;
    obj_new->elasticity = 0.9;
	obj_new->pos_x = 1.0;
	obj_new->pos_x = 0.0;
	obj_new->pos_y = 0.0;
	obj_new->pos_x_old = 0.0;
	obj_new->pos_y_old = 0.0;
	obj_new->vel_x = 0.0;
	obj_new->vel_y = 0.0;
	obj_new->acc_x = 0.0;
	obj_new->acc_y = 0.0;
	obj_new->acc_abs = 0.01;
	
    // carried by another object:
    obj_new->obj_carried_by = NULL;
    
    // carries another object:
    obj_new->obj_carries = NULL;
    
    // thrown by another object (usually after carried by):
    obj_new->obj_escape_col = NULL;
    
    // facing direction:
    obj_new->facing = OBJECT_FACING_SOUTH;
    
	// screen positions:
	obj_new->scr_pos_x = 0.0;
	obj_new->scr_pos_y = 0.0;
	
	// bitmaps:
	obj_new->surface = NULL;
	obj_new->wall = NULL;
	
    // tasks:
    obj_new->tsk = NULL;
    obj_new->is_security = false;
    obj_new->is_hunted_by_security = false;
    
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
    obj_new->anim_carry = false;
    obj_new->anim_pistol = false;
	
	// texts:
	obj_new->txt_language = (char *) malloc(3 * sizeof(char));
	obj_new->txt_language[0] = 'd';
	obj_new->txt_language[1] = 'e';
	obj_new->txt_language[2] = '\0';
	obj_new->txt = NULL;
	obj_new->txt_print = 0;
    obj_new->txt_next_letter = 0.0;
	obj_new->txt_surface = NULL;

	// waypoints:
	obj_new->ways = NULL;
	
	// collisions:
	obj_new->col = NULL;
    obj_new->col_sample_timer = 0;
    obj_new->disable_collision = false;
	
	return(obj_new);
}

walls_t* object_init_walls(SDL_Surface* surf_wall, SDL_Surface* surf) {
	walls_t* wall = (walls_t*) malloc(sizeof(walls_t));
    
	if (surf_wall != NULL) {
        
		wall->pxl = (uint8_t*) surf_wall->pixels;
        
		wall->x = 0;
		wall->y = surf->h - surf_wall->h;
		
        wall->x_shift = 0;
		wall->y_shift = 0;
        
		wall->w = surf_wall->w;
        wall->w_bmp = surface_get_bmp_width(surf_wall);
		wall->h = surf_wall->h;
		// find most left pixel:
		bool pixel_found = false;
		for (wall->lx = 0; wall->lx < wall->w; wall->lx++) {
			for (wall->ly = wall->h - 1; wall->ly > 0; wall->ly--) {
				if (wall->pxl[(wall->ly * wall->w_bmp) + wall->lx]) {
					pixel_found = true;
					break;
				}
			}
			if (pixel_found) {
				break;
			}
		}
        // go to bottom until black pixel found:
        wall->ly_beam = wall->ly;
        while (wall->ly_beam < surf_wall->h &&
            wall->pxl[(wall->ly_beam * wall->w_bmp) + wall->lx]) {
            wall->ly_beam++;
        }
        
		// find most right pixel:
		pixel_found = false;
		for (wall->rx = wall->w - 1; wall->rx >= 0; wall->rx--) {
			for (wall->ry = wall->h - 1; wall->ry > 0; wall->ry--) {
				if (wall->pxl[(wall->ry * wall->w_bmp) + wall->rx]) {
					pixel_found = true;
					break;
				}
			}
			if (pixel_found) {
				break;
			}
		}
        // go to bottom until black pixel found:
        wall->ry_beam = wall->ry;
        while (wall->ry_beam < surf_wall->h &&
            wall->pxl[(wall->ry_beam * wall->w_bmp) + wall->rx]) {
            wall->ry_beam++;
        }
        
		wall->slope = 
			((float) wall->ry - (float) wall->ly) / 
			((float) wall->rx - (float) wall->lx);
		
		wall->offset = 
			(float) wall->ly - 
			(float) wall->lx * wall->slope;
        
	} else {
		wall->pxl = NULL;
		wall->x = 0;
		wall->y = 0;
        wall->x_shift = 0;
		wall->y_shift = 0;
		wall->w = surf->w;
        wall->w_bmp = surface_get_bmp_width(surf);
		wall->h = surf->h;
		wall->rx = surf->w;
		wall->ry = 0;
        wall->ry_beam = surf->h + 1;
		wall->lx = 0;
		wall->ly = surf->h;
        wall->ly_beam = surf->h + 1;
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
    if (obj->ways != NULL) {
        object_free_waypoints(obj->ways);
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

void object_add_animation(object_t* obj, uint32_t id, float dt) {
	
    animation_t* anim = animation_init(dt);
    
    // place at first place in list and set as current animation:
    obj->anim = create_before(obj->anim, (void*) anim, id);
}

void object_select_animation(object_t* obj, uint32_t id) {
	
	obj->anim = find_id(obj->anim, id);
    if (obj->anim == NULL) {
        printf("WARNING! Missing animation %d for object %d!\n", id, obj->id);
        printf("Selecting first animation to avoid crash!\n");
        obj->anim = find_id(obj->anim, 1);
    }
    animation_t* anim = (animation_t*) obj->anim->entry;
    anim->time_active = 0.0;
}

void object_animate(object_t* obj, uint64_t frame, float dt) {
	
	// free original surface if it is not part of the animation:
	if (obj->anim_first_call) {
		obj->anim_first_call = 0;
		SDL_FreeSurface(obj->surface);
	}
	
    animation_t* anim = (animation_t*) obj->anim->entry;
    
	// get next picture:
	obj->surface = animation_get_next_surface(anim, frame);
    
    // increase time counter:    
    anim->time_active += dt;
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
    obj->txt_next_letter = 0.0;
	
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
    
    // place at last place in list:
    obj->ways = create_after(obj->ways, (void*) ways, id);
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
	
}

void object_get_next_waypoint(object_t* obj, float dt) {
	
    waypoints_t* ways = (waypoints_t*) obj->ways->entry;
    
	float pos_x_wp = ways->pos_x[ways->n];
	float pos_y_wp = ways->pos_y[ways->n];
    float border_x = ways->vel_abs[ways->n] + 5.0;
	float border_y = ways->vel_abs[ways->n] + 5.0;
	
	// object close enough to waypoint?
	if (obj->pos_x > pos_x_wp - border_x && 
		obj->pos_x < pos_x_wp + border_x &&
		obj->pos_y > pos_y_wp - border_y && 
		obj->pos_y < pos_y_wp + border_y ) {
        
        obj->vel_x = 0.0;
        obj->vel_y = 0.0;
        
        // disable fast animation switch protection:
        if (obj->anim_walk) {
            animation_t* anim = (animation_t*) obj->anim->entry;
            anim->time_active = 666.0;
        }
        
        if (ways->frames_wait[ways->n] > ways->frame) {
            ways->frame++;
            return;
        }
        ways->frame = 0;
        
		// select next waypoint:
		ways->n++;
		
		// cycle:
		if (ways->n == ways->num_ways) {
			ways->n = 0;
            if (!ways->is_cycle) {
                ways->active = false;
                return;
            }
		}
		
	} else {
        ways->frame = 0; // in case object is kicked out of waypoint
    }
}

void object_aim_for_waypoint(object_t* obj) {

    waypoints_t* ways = (waypoints_t*) obj->ways->entry;
    
    if (ways->frame != 0) {
        return;
    }
    
	float pos_x_wp = ways->pos_x[ways->n];
	float pos_y_wp = ways->pos_y[ways->n];
	float vel_x_wanted = pos_x_wp - obj->pos_x;
	float vel_y_wanted = pos_y_wp - obj->pos_y;
	
    // normalize:
    float norm = sqrtf(vel_x_wanted * vel_x_wanted + vel_y_wanted * vel_y_wanted);
    vel_x_wanted /= norm;
    vel_y_wanted /= norm;
        
    vel_x_wanted *= ways->vel_abs[ways->n];
    vel_y_wanted *= ways->vel_abs[ways->n];
        
    obj->vel_x = vel_x_wanted;
    obj->vel_y = vel_y_wanted;
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


void say(object_t* obj, uint32_t id, uint32_t duration) {
    
    object_select_text(obj, id);
    text_t* txt = (text_t*) obj->txt->entry;
    txt->duration = duration;
    obj->txt_print = 1;
}

bool said(object_t* obj) {
    
    return(obj->txt_print == 0);
}

// if calling say_new, you have to call say_free at a later step:
void say_new(object_t* obj, char* str, uint32_t duration) {
    
    object_add_text(obj, 0);
    text_t* txt = (text_t*) obj->txt->entry;
    text_add_string(txt, str);
    txt->duration = duration;
    obj->txt_print = 1;
}

void say_free(object_t* obj) {
    
    object_remove_text(obj, 0);
}

bool move_to_position(object_t* obj, float x, float y, float vel_abs) {
    
    const float x_border = 4.0;
    const float y_border = 4.0;
    
    float x_obj = obj->pos_x;
    float y_obj = obj->pos_y;
    
    
    if (x_obj > x - x_border &&
        x_obj < x + x_border &&
        y_obj > y - y_border &&
        y_obj < y + y_border) {
        
        // object reached targeted position
        return(true);
    
    }
        
    // set velocity toward position:
    x = x - x_obj;
    y = y - y_obj;
    float norm = sqrtf(x * x + y * y);
    obj->vel_x = vel_abs * x / norm;
    obj->vel_y = vel_abs * y / norm;
    
    return(false);
}

bool move_to_relative(object_t* obj, float x, float y, float vel_abs) {
    // not implemented yet:
    return (false);
}

void face(object_t* obj, object_t* obj_target, float dt) {
    
    float distance_x = obj_target->pos_x - obj->pos_x;
    float distance_y = obj_target->pos_y - obj->pos_y;
    
    if (fabsf(distance_x) > fabsf(distance_y)) {
        
        if (distance_x < 0) {
            obj->facing = OBJECT_FACING_WEST;
        } else {
            obj->facing = OBJECT_FACING_EAST;
        }
        
    } else {
        
        if (distance_y < 0) {
            obj->facing = OBJECT_FACING_NORTH;
        } else {
            obj->facing = OBJECT_FACING_SOUTH;
        }
    }
}

void stop(object_t* obj) {
    
    if (obj->id == OBJECT_HERO_ID) {
        obj->can_move = false;
    } else {
        if (obj->ways != NULL) {
            waypoints_t* ways = (waypoints_t*) obj->ways->entry;
            ways->active = false;
        }
    }
    //obj->anim_walk = false;
    obj->vel_x = 0.0;
    obj->vel_y = 0.0;
}

void move_on(object_t* obj) {
    
    if (obj->id == OBJECT_HERO_ID) {
        obj->can_move = true;
    } else {
        if (obj->ways != NULL) {
            waypoints_t* ways = obj->ways->entry;
            ways->active = true;
        }
    }
    //obj->anim_walk = true;
}

meter_t* meter_get(object_t* obj, uint8_t type) {
    
    meter_t* mtr = NULL;
    
    if (obj->mtr != NULL) {
        list_t* lst = (list_t*) obj->mtr;
        lst = find_id(lst, type);
        if (lst != NULL) {
            mtr = (meter_t*) lst->entry;
        }
    }
    
    return(mtr);
}

void drink_beer(object_t* obj, int16_t value) {
    
    meter_t* mtr = meter_get(obj, METER_BEER);
    meter_update(mtr, mtr->value + value);
}

void change_mood(object_t* obj, int16_t value) {
    
    meter_t* mtr = meter_get(obj, METER_MOOD);
    meter_update(mtr, mtr->value + value);
}

bool check_collision(object_t* obj1, uint32_t id_obj2) {
    
    if (obj1->col != NULL) {
        if (find_id(obj1->col, id_obj2) != NULL) {
            return(true);
        }
    }
    
    return(false);
}

bool pick_up(object_t* obj, object_t* obj_target) {
    
    // don't pick up what is carried by another object:
    if (obj_target->obj_carried_by != NULL) {
        return(false);
    }
    
    // don't pick up what carries another object:
    if (obj_target->obj_carries != NULL) {
        return(false);
    }
    
    // don't pick up what has collisions disabled:
    /*if (obj_target->disable_collision) {
        return(false);
    }*/
    
    obj->obj_carries = obj_target;
    obj_target->obj_carried_by = obj;
    
    obj_target->pos_x = obj->pos_x;
    obj_target->pos_y = obj->pos_y;
    
    if (obj_target->obj_escape_col == obj) {
        obj_target->obj_escape_col = NULL;
    }
    
    return(true);
}

void throw(object_t* obj, float vel_add) {
    
    object_t* obj_target = obj->obj_carries; // object to be thrown
    obj_target->obj_escape_col = obj;
    obj->obj_carries = NULL;
    obj_target->obj_carried_by = NULL;
    
    float offset = (float) obj->wall->w;
    if (offset < (float) obj->wall->h) {
        offset = (float) obj->wall->h;
    }
    
    float vel_x = obj->vel_x;
    float vel_y = obj->vel_y;
    
    if (vel_x != 0.0 || vel_y != 0.0) {
        
        float norm = sqrtf(vel_x * vel_x + vel_y * vel_y);
        float norm_x = vel_x / norm;
        float norm_y = vel_y / norm;
        obj_target->vel_x = vel_x + vel_add * norm_x;
        obj_target->vel_y = vel_y + vel_add * norm_y;
        
    } else {
        
        switch (obj->facing) {
            case OBJECT_FACING_NORTH:
                obj_target->vel_x = 0.0;
                obj_target->vel_y = -vel_add;
                break;
            case OBJECT_FACING_SOUTH:
                obj_target->vel_x = 0.0;
                obj_target->vel_y = vel_add;
                break;
            case OBJECT_FACING_WEST:
                obj_target->vel_x = -vel_add;
                obj_target->vel_y = 0.0;
                break;
            case OBJECT_FACING_EAST:
                obj_target->vel_x = vel_add;
                obj_target->vel_y = 0.0;
                break;
        }
    }
    
    switch (obj->facing) {
        case OBJECT_FACING_NORTH:
            obj_target->pos_y = obj->pos_y + obj->surface->h / 2
                - obj_target->surface->h / 2;
            break;
        case OBJECT_FACING_SOUTH:
            obj_target->pos_y = obj->pos_y + obj->surface->h / 2
                - obj_target->surface->h / 2;
            break;
        case OBJECT_FACING_WEST:
            obj_target->pos_x = obj->pos_x + obj->surface->w / 2
                - obj_target->surface->w / 2;
            break;
        case OBJECT_FACING_EAST:
            obj_target->pos_x = obj->pos_x + obj->surface->w / 2
                - obj_target->surface->w / 2;
            break;
    }
}

void start_waypoints(object_t* obj, uint32_t id) {
    
    object_select_waypoints(obj, id);
    waypoints_t* ways = (waypoints_t*) obj->ways->entry;
    ways->n = 0;
    ways->active = true;
    obj->can_move = true;
    if (count(obj->anim) >= 8) {
        obj->anim_walk = true;
    }
}

bool waypoints_finished(object_t* obj) {
    
    waypoints_t* ways = (waypoints_t*) obj->ways->entry;
    
    return(!ways->active);
}

float squared_distance(object_t* obj1, object_t* obj2) {
    
    float dist_x = obj1->pos_x - obj2->pos_x;
    float dist_y = obj1->pos_y - obj2->pos_y;
    
    return(dist_x * dist_x + dist_y * dist_y);
}

bool squared_distance_smaller(
    object_t* obj1, object_t* obj2, float dist_squared) {
    
    float dist_x = obj1->pos_x - obj2->pos_x;
    float dist_y = obj1->pos_y - obj2->pos_y;
    
    return(dist_x * dist_x + dist_y * dist_y < dist_squared);
}

bool squared_distance_greater(
    object_t* obj1, object_t* obj2, float dist_squared) {
    
    float dist_x = obj1->pos_x - obj2->pos_x;
    float dist_y = obj1->pos_y - obj2->pos_y;
    
    return(dist_x * dist_x + dist_y * dist_y > dist_squared);
}
