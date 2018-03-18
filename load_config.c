#include "load_config.h"

configentry* load_config_defaults(configentry* entry, char* path, object_t* obj) {
	
	SDL_Surface* surf = NULL;
	SDL_Surface* surf_wall = NULL;
	char* file_name;
	
	
	while (entry != NULL) {
		
		if        (strcmp(entry->key, "object") == 0) {
			
			obj->id = atoi(entry->value);
			printf("Object %d added:\n", obj->id);
			entry = entry->next;
			
		} else if (strcmp(entry->key, "surface") == 0) {
			
			file_name = entry->value;
			strncpy(path, "objects", 100);
			strncat(path, "/", 100);		// TODO: Windows compatible
			strncat(path, file_name, 100);
			surf = surface_on_load(path);
			if(surf == NULL) {
				printf("Warning: Problem loading config file.\n");
				printf("File %s not found.\n", file_name);
			}
			obj->surface = surf;
			entry = entry->next;
			
		} else if (strcmp(entry->key, "walls") == 0) {
			
			strncpy(path, "objects", 100);
			strncat(path, "/", 100);		// TODO: Windows compatible
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
			
		} else {
			break;
		}
		
	}
	
	if (surf != NULL && surf_wall != NULL) {
		obj->wall = object_init_walls(surf_wall, surf);
	}
	
	return(entry);
	
}
