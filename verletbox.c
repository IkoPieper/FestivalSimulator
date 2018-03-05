#include "verletbox.h"

verletbox_t* verletbox_init(object_t* obj) {
	
	verletbox_t* vbox = (verletbox_t*) malloc(sizeof(verletbox_t));
	
	int x, y;
	
	obj = object_get(obj, OBJECT_BACKGROUND_ID);
	
	// width and height wanted:
	vbox->w = 500;
	vbox->h = 500;
	
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
	
	
	vbox->boxes = boxes;
	
	obj = object_get_first(obj);
	
	while (obj != NULL) {
		
		if (obj->id != OBJECT_SURFDISPLAY_ID &&
			obj->id != OBJECT_BACKGROUND_ID) {
		
			x = obj->pos_x / vbox->w;
			y = obj->pos_y / vbox->h;
			
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
	
	return(vbox);
}
