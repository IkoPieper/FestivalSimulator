#ifndef _VERLETBOX_H_
    #define _VERLETBOX_H_

#include "object.h"

typedef struct verletbox verletbox_t;

struct verletbox {
	
	unsigned int w;	            // box width / pxl
	unsigned int h;             // box height / pxl
	unsigned int num_w;	        // number of boxes in x direction
	unsigned int num_h;         // number of boxes in y direction
	
	object_t*** boxes;
	
};

verletbox_t* verletbox_init(object_t* obj);
void verletbox_update(verletbox_t* vbox, object_t* obj);
object_t* verletbox_get_first_object(object_t* obj);

#endif
