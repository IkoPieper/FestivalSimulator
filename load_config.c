#include "load_config.h"

configentry* load_config_defaults(configentry* entry, char* path, object_t* obj) {
	
	SDL_Surface* surf = NULL;
	SDL_Surface* surf_wall = NULL;
	char* file_name;
	
	
	while (entry != NULL) {
		
		if        (strcmp(entry->key, "object") == 0) {
			
			obj->id = atoi(entry->value);
			printf("Object %d added\n", obj->id);
			entry = entry->next;
			
		} else if (strcmp(entry->key, "surface") == 0) {
			
			file_name = entry->value;
			strncpy(path, "objects", 100);
			strncat(path, "/", 100);
			strncat(path, file_name, 100);
			surf = surface_on_load(path);
			if(surf == NULL) {
				printf("Warning: Problem loading config file.\n");
				printf("File %s not found.\n", file_name);
			}
			obj->surface = surf;
			entry = entry->next;
			
		} else if (strcmp(entry->key, "walls") == 0) {
			
			file_name = entry->value;
			strncpy(path, "objects", 100);
			strncat(path, "/", 100);
			strncat(path, file_name, 100);
			surf_wall = surface_on_load(path);
			if(surf_wall == NULL) {
				printf("Warning: Problem loading config file.\n");
				printf("File %s not found.\n", file_name);
			}
			entry = entry->next;
		
		} else if (strcmp(entry->key, "pos_x") == 0) {
			
			obj->pos_x = atof(entry->value);
			entry = entry->next;
			
		} else if (strcmp(entry->key, "pos_y") == 0) {
			
			obj->pos_y = atof(entry->value);
			entry = entry->next;
			
		} else if (strcmp(entry->key, "can_move") == 0) {
			
			obj->can_move = atoi(entry->value);
			entry = entry->next;
			
		} else if (strcmp(entry->key, "mass") == 0) {
			
			obj->mass = atof(entry->value);
			entry = entry->next;
			
		} else if (strcmp(entry->key, "damping") == 0) {
			
			obj->damping = atof(entry->value);
			entry = entry->next;
			
		} else if (strcmp(entry->key, "acc_abs") == 0) {
			
			obj->acc_abs = atof(entry->value);
			entry = entry->next;
            
        } else if (strcmp(entry->key, "disable_collision") == 0) {
			
			obj->disable_collision = atoi(entry->value);
			entry = entry->next;
            
        } else if (strcmp(entry->key, "render_early") == 0) {
			
			obj->render_early = atoi(entry->value);
			entry = entry->next;
            
        } else if (strcmp(entry->key, "anim_walk") == 0) {
			
			obj->anim_walk = atoi(entry->value);
			entry = entry->next;
			
		} else {
			break;
		}
		
	}
	
	if (surf != NULL) {
		obj->wall = object_init_walls(surf_wall, surf);
	}
	
	return(entry);
	
}

configentry* load_config_animation(configentry* entry, char* path, object_t* obj) {
	
	SDL_Surface* surf = NULL;
	char* file_name;
	
	
	while (entry != NULL) {
		
		if        (strcmp(entry->key, "animation") == 0) {
			
			object_add_animation(obj, atoi(entry->value));
			printf("Animation %d added\n", atoi(entry->value));
			entry = entry->next;
			
		} else if (strcmp(entry->key, "delay_frames") == 0) {
			
			obj->anim->delay_frames = atoi(entry->value);
			entry = entry->next;
			
		} else if (strcmp(entry->key, "surface") == 0) {
			
			file_name = entry->value;
			strncpy(path, "objects", 100);
			strncat(path, "/", 100);
			strncat(path, file_name, 100);
			surf = surface_on_load(path);
			if(surf == NULL) {
				printf("Warning: Problem loading config file.\n");
				printf("File %s not found.\n", file_name);
			}
			animation_add_surface(obj->anim, surf);
			entry = entry->next;
			
		} else {
			break;
		}
		
	}
	
	return(entry);
	
}

configentry* load_config_waypoints(configentry* entry, char* path, object_t* obj) {
	
    uint32_t num_waypoints = 0;
    uint32_t id = atoi(entry->value);
    entry = entry->next;
    
    if (strcmp(entry->key, "num_waypoints") == 0) {
        num_waypoints = atoi(entry->value);
        object_add_waypoints(obj, id, num_waypoints);
        entry = entry->next;
    } else {
        printf("Error reading waypoints of object %d\n! skipping...", obj->id);
        return (entry);
    }
    
    if (strcmp(entry->key, "pos_are_relative") == 0) {
        obj->ways->pos_are_relative = atoi(entry->value);
        entry = entry->next;
    }
    
    for (uint32_t n = 0; n < num_waypoints; n++) {
        
        for (uint8_t m = 0; m < 4; m++) {
	
            if (strcmp(entry->key, "vel_abs") == 0) {
                obj->ways->vel_abs[n] = atof(entry->value);
                entry = entry->next;
            } else if (strcmp(entry->key, "pos_x") == 0) {
                obj->ways->pos_x[n] = atof(entry->value);
                entry = entry->next;
            } else if (strcmp(entry->key, "pos_y") == 0) {
                obj->ways->pos_y[n] = atof(entry->value);
                entry = entry->next;
            } else if (strcmp(entry->key, "frames_wait") == 0) {
                obj->ways->frames_wait[n] = atof(entry->value);
                entry = entry->next;
            } else {
                printf("Error reading waypoints of object %d\n! skipping...", obj->id);
                return (entry);
            }
		}
        
    }
	
    object_activate_waypoints(obj);
    
	return(entry);
	
}

configentry* load_config_text(configentry* entry, object_t* obj) {
	
	while (entry != NULL) {
		
		if        (strcmp(entry->key, "text") == 0) {
			
			object_add_text(obj, atoi(entry->value));
			printf("Text %d added\n", atoi(entry->value));
			entry = entry->next;
			
		} else if (strcmp(entry->key, "de") == 0) {
			
			if (strcmp(obj->txt_language, "de") == 0) {
				text_add_string(obj->txt, entry->value);
			}
			entry = entry->next;
			
			fprintf(stdout, "%s\n", obj->txt->str);
			
		} else {
			break;
		}
		
	}
	
	return(entry);
	
}
