#include "verletbox.h"

verletbox_t* verletbox_init(object_t* obj) {
	
	verletbox_t* vbox = (verletbox_t*) malloc(sizeof(verletbox_t));
	
	int x, y;
	
	obj = object_get(obj, OBJECT_BACKGROUND_ID);
	
	// width and height wanted:
	vbox->w = obj->surface->w;
	vbox->h = obj->surface->h;
	
	// number of boxes:
	vbox->num_w = obj->surface->w / vbox->w + 1;
	vbox->num_h = obj->surface->h / vbox->h + 1;
	
	// width and height corrected:
	vbox->w = obj->surface->w / vbox->num_w + 1;
	vbox->h = obj->surface->h / vbox->num_h + 1;
	
	object_t*** boxes = (object_t***) malloc(vbox->num_w * sizeof(object_t**));
	
	for (x = 0; x < vbox->num_w; x++) {
		boxes[x] = (object_t**) malloc(vbox->num_h * sizeof(object_t*));
		for (y = 0; y < vbox->num_h; y++) {
			boxes[x][y] = NULL;
		}
	}
	
	obj = object_get_first(obj);
	
	while (obj != NULL) {
		
		if (obj->id != OBJECT_SURFDISPLAY_ID &&
			obj->id != OBJECT_BACKGROUND_ID) {
		
			x = obj->pos_x / vbox->w;
			y = obj->pos_y / vbox->h;
			
			obj->vbox_x = x;
			obj->vbox_y = y;
			
			if (boxes[x][y] == NULL) {
				obj->prev_vbox = NULL;
				obj->next_vbox = NULL;
				boxes[x][y] = obj;
			} else {
				object_t* obj2 = boxes[x][y];
				obj->prev_vbox = NULL;
				obj->next_vbox = obj2;
				obj2->prev_vbox = obj;
				boxes[x][y] = obj;
			}
		
		}
		obj = obj->next_object;
	}
	
	vbox->boxes = boxes;
	
	return(vbox);
}


void verletbox_update(verletbox_t* vbox, object_t* obj) {
	
	obj = object_get_first(obj);
	unsigned int x, y, x_old, y_old;
	
	while (obj != NULL) {
		
		if ((obj->vel_x != 0.0 || obj->vel_y != 0.0) && // object moved?
			obj->id != OBJECT_SURFDISPLAY_ID &&
			obj->id != OBJECT_BACKGROUND_ID) {
		
			// calculate new vbox:
			x_old = obj->vbox_x;
			y_old = obj->vbox_y;			
			x = obj->pos_x / vbox->w;
			y = obj->pos_y / vbox->h;
			
			// check if vbox changed:
			if (x_old != x || y_old != y) {
				// move to another vbox:
				
				obj->vbox_x = x;
				obj->vbox_y = y;
				
				// remove from list:
				if (obj->prev_vbox != NULL && obj->next_vbox != NULL) {
					obj->prev_vbox->next_vbox = obj->next_vbox;
					obj->next_vbox->prev_vbox = obj->prev_vbox;
					if (vbox->boxes[x_old][y_old] == obj) {
						vbox->boxes[x_old][y_old] = obj->prev_vbox;
					}
				} else if (obj->prev_vbox == NULL && obj->next_vbox == NULL) {
					vbox->boxes[x_old][y_old] = NULL;
				} else if (obj->prev_vbox == NULL) {
					obj->next_vbox->prev_vbox = obj->prev_vbox;
					if (vbox->boxes[x_old][y_old] == obj) {
						vbox->boxes[x_old][y_old] = obj->next_vbox;
					}
				} else if (obj->next_vbox == NULL) {
					obj->prev_vbox->next_vbox = obj->next_vbox;
					if (vbox->boxes[x_old][y_old] == obj) {
						vbox->boxes[x_old][y_old] = obj->prev_vbox;
					}
				}
				
				fprintf(stderr, "obj->id: %d\n", obj->id);
				fprintf(stderr, "obj->pos_x: %f, obj->pos_y: %f\n", obj->pos_x, obj->pos_y);
				fprintf(stderr, "x: %d, y: %d\n", x, y);
				fprintf(stderr, "vbox->num_w: %d, vbox->num_h: %d\n", vbox->num_w, vbox->num_h);
				
				// add to another list:
				if (vbox->boxes[x][y] == NULL) {
					obj->prev_vbox = NULL;
					obj->next_vbox = NULL;
					vbox->boxes[x][y] = obj;
				} else {
					object_t* obj2 = vbox->boxes[x][y];
					obj->prev_vbox = NULL;
					obj->next_vbox = obj2;
					fprintf(stderr, "obj2->id: %d\n", obj2->id);
					obj2->prev_vbox = obj;
					vbox->boxes[x][y] = obj;
				}
				
				
				/*printf("vbox changed, obj->id: %d \n", obj->id);
				
				printf("x_old: %d, x: %d\n", x_old, x);
				printf("y_old: %d, y: %d\n", y_old, y);
				
				printf("\n");
				fprintf(stderr, "vbox->num_w: %d, vbox->num_h: %d\n", vbox->num_w, vbox->num_h);
				for (int y = 0; y < vbox->num_h; y++) {
					for (int x = 0; x < vbox->num_w; x++) {
						object_t* obj_tmp = vbox->boxes[x][y];
						while (obj_tmp != NULL) {
							printf("%d:", obj_tmp->id);
							obj_tmp = obj_tmp->next_vbox;
						}
						printf(", ");
					}
					printf("\n");
				}
				printf("\n");*/
			}
		
		}
		obj = obj->next_object;
	}
	
}


object_t* verletbox_get_first_object(object_t* obj) {
	
	if (obj != NULL) {
		while (obj->prev_vbox != NULL) {
			obj = obj->prev_vbox;
		}
	}
	return(obj);
	
}
