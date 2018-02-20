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

object_t* object_get_first_render(object_t* obj) {
	
	if (obj != NULL) {
		while(obj->prev_render != NULL) {
			obj = obj->prev_render;
		}
	}
	
	return(obj);
}

object_t* object_add(object_t* obj, unsigned int id) {
	
	object_t* obj_new = (object_t*) malloc(sizeof(object_t));
	
	obj = object_get_last(obj);
	obj_new->prev_object = obj;
	obj_new->prev_render = obj;
	obj_new->next_object = NULL;
	obj_new->next_render = NULL;
	obj_new->render_id = 0;
	
	if (obj != NULL) {
		obj->next_object = obj_new;
		obj->next_render = obj_new;
	} 
	
	obj_new->id = id;
	
	obj_new->anim = NULL;
	obj_new->anim_prev = NULL;
	obj_new->anim_next = NULL;
	
	obj_new->mass = 1.0;
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
	obj_new->damping = 0.0;
	obj_new->scr_pos_x = 0;
	obj_new->scr_pos_y = 0;
	obj_new->scr_pos_x_old = 0;
	obj_new->scr_pos_y_old = 0;
	obj_new->min_scr_pos_x = -99999;
	obj_new->max_scr_pos_x = 99999;
	obj_new->min_scr_pos_y = -99999;
	obj_new->max_scr_pos_y = 99999;
	obj_new->surface = NULL;
	obj_new->walls = NULL;
	obj_new->font = NULL;
	
	obj_new->col = NULL;
    obj_new->disable_collision = 0;
    obj_new->collision_stop = 0;
	
	return(obj_new);
}

object_t* object_remove(object_t* obj, unsigned int id) {
	
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
	
	free(obj);
	
	return(ret);
}

void object_clear(object_t* obj) {
	
	while (obj->id != 0) {
		obj = object_remove(obj, obj->id);
	}
}

object_t* object_get(object_t* obj, unsigned int id) {
	
	obj = object_get_last(obj);
	//fprintf(stderr, "object id = %d\n");
	while (obj->id != id) {
		obj = obj->prev_object;
	}
	
	return(obj);
}

unsigned int object_get_count(object_t* obj) {
	
	obj = object_get_last(obj);
	unsigned int count = 0;
	
	while (obj != NULL) {
		count++;
		obj = obj->prev_object;
	}
	
	return(count);
}

void object_add_animation(object_t* obj, unsigned int id) {
	
	animation_t* anim = animation_init(id);
	
	// place at first place in list:
	anim->next = obj->anim;
	
	if (obj->anim != NULL) {
		obj->anim->prev = anim;
	}
	
	// set as current animation:
	obj->anim = anim;
	
}

void object_select_animation(object_t* obj, unsigned int id) {
	
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

void object_remove_animation(object_t* obj, unsigned int id) {
	
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
}

void object_remove_selected_animation(object_t* obj) {
	
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
