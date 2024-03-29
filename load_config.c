#include "load_config.h"

configentry* load_config_defaults(configentry* entry, object_t* obj) {
    
    SDL_Surface* surf = NULL;
    SDL_Surface* surf_wall = NULL;
    char path[100];
    char* file_name;
    
    while (entry != NULL) {
        
        if        (strcmp(entry->key, "object") == 0) {
            
            obj->id = atoi(entry->value);
            printf("Object %d added\n", obj->id);
            entry = entry->next;
        } else if (strcmp(entry->key, "task") == 0) {
            
            tasks_add_to_object(obj, atoi(entry->value));
            entry = entry->next;
            
        } else if (strcmp(entry->key, "has_item") == 0) {
            // add id of object that acts like an item. since objects 
            // are not initialized yet, the direct pointer to the object
            // will be added later in on_init_items():
            obj->itm = create_after(obj->itm, NULL, atoi(entry->value));
            entry = entry->next;
            
        } else if (strcmp(entry->key, "surface") == 0) {
            
            file_name = entry->value;
            strncpy(path, "objects", 8);
            strncat(path, "/", 2);
            strncat(path, file_name, 100-8-1);
            surf = surface_on_load(path);
            if(surf == NULL) {
                printf("Error: File %s not found.\n", file_name);
                exit(0);
            }
            obj->surface = surf;
            entry = entry->next;
            
        } else if (strcmp(entry->key, "walls") == 0) {
            
            file_name = entry->value;
            strncpy(path, "objects", 8);
            strncat(path, "/", 2);
            strncat(path, file_name, 100-8-1);
            surf_wall = surface_on_load(path);
            if(surf_wall == NULL) {
                printf("Error: File %s not found.\n", file_name);
                exit(0);
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
            
        } else if (strcmp(entry->key, "elasticity") == 0) {
            
            obj->elasticity = atof(entry->value);
            entry = entry->next;
            
        } else if (strcmp(entry->key, "acc_abs") == 0) {
            
            obj->acc_abs = atof(entry->value);
            entry = entry->next;
            
        } else if (strcmp(entry->key, "disable_collision") == 0) {
            
            obj->disable_collision = atoi(entry->value);
            entry = entry->next;
            
        } else if (strcmp(entry->key, "disable_render") == 0) {
            
            obj->disable_render = atoi(entry->value);
            entry = entry->next;
            
        } else if (strcmp(entry->key, "render_early") == 0) {
            
            obj->render_early = atoi(entry->value);
            entry = entry->next;
            
        } else if (strcmp(entry->key, "anim_walk") == 0) {
            
            obj->anim_walk = atoi(entry->value);
            entry = entry->next;
            
        } else if (strcmp(entry->key, "anim_carry") == 0) {
            
            obj->anim_carry = atoi(entry->value);
            entry = entry->next;
            
        } else if (strcmp(entry->key, "anim_pistol") == 0) {
            
            obj->anim_carry = atoi(entry->value);
            entry = entry->next;
            
        } else if (strcmp(entry->key, "is_security") == 0) {
            
            obj->is_security = atoi(entry->value);
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

configentry* load_config_item(configentry* entry, object_t* obj) {
    
    SDL_Surface* surf = NULL;
    char path[100];
    char* file_name;
    
    while (entry != NULL) {
        
        if        (strcmp(entry->key, "item") == 0) {
            
            printf("ITEM ADDED: %d\n", atoi(entry->value));
            items_init_object_item_props(obj, NULL, atoi(entry->value));
            entry = entry->next;
            
        } else if (strcmp(entry->key, "item_surface") == 0) {
            
            file_name = entry->value;
            strncpy(path, "objects", 8);
            strncat(path, "/", 2);
            strncat(path, file_name, 100-8-1);
            surf = surface_on_load(path);
            if(surf == NULL) {
                printf("Warning: Problem loading config file.\n");
                printf("File %s not found.\n", file_name);
            }
            
            obj->itm_props->surf = surf;
            
            entry = entry->next;
            
        } else {
            break;
        }
        
    }
    
    return(entry);
    
}

configentry* load_config_animation(
    configentry* entry, object_t* obj, float dt) {
    
    SDL_Surface* surf = NULL;
    char path[100];
    char* file_name;
    
    while (entry != NULL) {
        
        if        (strcmp(entry->key, "animation") == 0) {
            
            object_add_animation(obj, atoi(entry->value), dt);
            printf("Animation %d added\n", atoi(entry->value));
            entry = entry->next;
            
        } else if (strcmp(entry->key, "delay_frames") == 0) {
            
            animation_t* anim = (animation_t*) obj->anim->entry;
            
            anim->delay_frames = (uint32_t) (atof(entry->value) / dt);
            if (anim->delay_frames < 1) {
                anim->delay_frames = 1;
            }
            anim->delay_frames_wanted = anim->delay_frames;
            
            entry = entry->next;
            
        } else if (strcmp(entry->key, "surface") == 0) {
            
            file_name = entry->value;
            strncpy(path, "objects", 8);
            strncat(path, "/", 2);
            strncat(path, file_name, 100-8-1);
            surf = surface_on_load(path);
            if(surf == NULL) {
                printf("Warning: Problem loading config file.\n");
                printf("File %s not found.\n", file_name);
            }
            animation_add_surface((animation_t*) obj->anim->entry, surf);
            entry = entry->next;
            
        } else {
            break;
        }
        
    }
    
    return(entry);
    
}

configentry* load_config_waypoints(
    configentry* entry, object_t* obj, float dt) {
    
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
    
    waypoints_t* ways = (waypoints_t*) obj->ways->entry;
    
    while (strcmp(entry->key, "vel_abs") != 0) {
        
        if (strcmp(entry->key, "active") == 0) {
            ways->active = atoi(entry->value);
        } else if (strcmp(entry->key, "pos_are_relative") == 0) {
            ways->pos_are_relative = atoi(entry->value);
        } else if (strcmp(entry->key, "is_cycle") == 0) {
            ways->is_cycle = atoi(entry->value);
        }
        
        entry = entry->next;
    }
    
    for (uint32_t n = 0; n < num_waypoints; n++) {
        
        for (uint8_t m = 0; m < 4; m++) {
    
            if (strcmp(entry->key, "vel_abs") == 0) {
                ways->vel_abs[n] = atof(entry->value);
            } else if (strcmp(entry->key, "pos_x") == 0) {
                ways->pos_x[n] = atof(entry->value);
            } else if (strcmp(entry->key, "pos_y") == 0) {
                ways->pos_y[n] = atof(entry->value);
            } else if (strcmp(entry->key, "frames_wait") == 0) {
                ways->frames_wait[n] = (uint32_t) (atof(entry->value) / dt);
                if (ways->frames_wait[n] < 1) {
                    ways->frames_wait[n] = 1;
                }
            } else {
                printf("Error reading waypoints of object %d\n! skipping...", obj->id);
                return (entry);
            }
            entry = entry->next;
        }
        
    }
    
    object_activate_waypoints(obj);
    
    // select the first waypoints:
    obj->ways = get_first(obj->ways);
    
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
                text_t* txt = (text_t*) obj->txt->entry;
                text_add_string(txt, entry->value);
                
                fprintf(stdout, "%s\n", txt->str);
            }
            entry = entry->next;
            
        } else {
            break;
        }
        
    }
    
    return(entry);
    
}
