#include "on_init.h"

object_t* on_init_objects(video_t* vid, float dt, uint8_t area) {
	
	// init objects:
	object_t* obj = NULL;
	
	obj = object_add(obj, OBJECT_BACKGROUND_ID);	// background
	
	if (on_init_background(obj, vid, area)) {
		object_clean_up(obj);
		return(NULL);
	}
	if (on_init_objects_config(obj, dt, area)) {
		object_clean_up(obj);
		return(NULL);
	}
	if (on_init_hero(obj, vid)) {
		object_clean_up(obj);
		return(NULL);
	}
    if (on_init_items(obj)) {
		object_clean_up(obj);
		return(NULL);
	}

	// init old positions:
	object_t* obj_tmp = object_get_first(obj);
	while (obj_tmp != NULL) {
		
		obj_tmp->pos_x_old = obj_tmp->pos_x;
		obj_tmp->pos_y_old = obj_tmp->pos_y;
		
		// get next object obj:
		obj_tmp = obj_tmp->next_object;
	}	
	
	return(object_get_first(obj));
}

bool on_init_background(object_t* obj, video_t* vid, uint8_t area) {
	
	obj = object_get(obj, OBJECT_BACKGROUND_ID);
	
	obj->has_moved = 0;
	obj->mass = 99999999999.0;
	obj->damping = 1.0;
	
    char path[64];
    snprintf(path, 64, "objects/area%d/background_walls.bmp", area);
    
	SDL_Surface* surf_wall;
	if((surf_wall = surface_on_load(path)) == NULL) {
        printf("Error loading file %s\n", path);
		exit(0);
	}
    
    snprintf(path, 64, "objects/area%d/background.bmp", area);
    
	if((obj->surface = surface_on_load(path)) == NULL) {
        printf("Error loading file %s\n", path);
		exit(0);
	}
	obj->wall = object_init_walls(surf_wall, obj->surface);
	
	obj->pos_x = 0.0;	// the background defines the positions
	obj->pos_y = 0.0;
	obj->scr_pos_x = -obj->surface->w / 2 + vid->surface->w / 2;
	obj->scr_pos_y = -obj->surface->h / 2 + vid->surface->h / 2;
	
	return(false);
}

bool on_init_hero(object_t* obj, video_t* vid) {
	
	// see objects/area?/hero.txt for most of the initialization
	
	obj = object_get(obj, OBJECT_HERO_ID);
	
	// start position is middle of screen:
	obj->scr_pos_x = vid->surface->w / 2 - obj->surface->w / 2;
	obj->scr_pos_y = vid->surface->h / 2 - obj->surface->h / 2;
    
    // object meters:
    object_add_meter(obj, METER_BEER, METER_BEER,   10, 10);
    object_add_meter(obj, METER_MOOD, METER_MOOD,   10, 40);
    object_add_meter(obj, METER_URIN, METER_URIN,   10, 70);
    object_add_meter(obj, METER_POINTS, METER_POINTS, 10, 100);
    object_add_meter(obj, METER_ITEM, METER_ITEM, 10, 130);
    
	return(false);
}
	
bool on_init_objects_config(object_t* obj, float dt, uint8_t area) {
	
	DIR* hdl_dir;
	struct dirent* dir;
	char path[100];
	char path_area[14];
    snprintf(path_area, 14, "objects/area%d", area);
    
	hdl_dir = opendir(path_area);
	
	while ((dir = readdir(hdl_dir)) != NULL) {
      
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..") &&
			strstr(dir->d_name, ".txt") != NULL) {

	
			// build path to file:
			strncpy(path, path_area, 100-1);
			strncat(path, "/", 2);
			strncat(path, dir->d_name, 100-1);
	
			configentry* data = conf_load_data(path);
			configentry* entry = data;

			if (data == NULL) {
				printf("Warning: Unable to load config file. Use Default Settings.\n");
				return(true);
			}
			
			obj = object_add(obj, 666666666);
			
			while (entry != NULL) {
			
				if        (strcmp(entry->key, "object") == 0) {
                    
					entry = load_config_defaults(entry, obj);
                    
                } else if (strcmp(entry->key, "item") == 0) {
                    
					entry = load_config_item(entry, obj);
                    
				} else if (strcmp(entry->key, "animation") == 0) {
                    
					entry = load_config_animation(entry, obj, dt);
                    
                } else if (strcmp(entry->key, "waypoints") == 0) {
                    
					entry = load_config_waypoints(entry, obj, dt);
                    
				} else if (strcmp(entry->key, "text") == 0) {
                    
					entry = load_config_text(entry, obj);
                    
				} else {
                    
                    printf("ERROR: Unknown key %s in %s!\n", entry->key, path);
                    exit(1);
                }
			}
			
			conf_free_data(data);
		}
	}
    
    closedir(hdl_dir);
	
	return(false);
}

// fill existing item id lists with object pointers used for faster
// access in main loop:
bool on_init_items(object_t* obj) {
	
    obj = object_get_first(obj);
    
    while (obj != NULL) {
        
        if (obj->itm != NULL) {
            list_t* lst = obj->itm;
            lst = get_last(lst);
            
            while (lst != NULL) {
                object_t* obj_item = object_get(obj, lst->id);
                lst->entry = (void*) obj_item;
                lst->id = obj_item->itm_props->id;
                obj_item->disable_collision = true;
                obj_item->disable_render = true;
                obj_item->can_move = false;
                printf("on_init_items() id: %d\n", obj_item->id);
                lst = lst->prev;
            }
        }
        
        obj = obj->next_object;
    }
    
	return(false);
}
