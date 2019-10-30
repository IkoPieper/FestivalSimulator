#include "on_render.h"
 
void on_render(object_t* obj) {
	
	//Uint32 time;
	object_t* obj_first = object_get_first(obj);
    
    on_render_sort(obj);
    
	
	// draw objects on surf display:
	object_t* obj_dsp = object_get(obj, OBJECT_SURFDISPLAY_ID);
	
    
    // render background first:
    object_t* obj_bg = object_get(obj, OBJECT_BACKGROUND_ID);
    surface_on_draw(
                obj_dsp->surface, 
                obj_bg->surface, 
                (int) obj_bg->scr_pos_x, 
                (int) obj_bg->scr_pos_y);
    
    
    // render early:
    obj = obj_first;
    
	while (obj != NULL) {
        
        if (obj->id != OBJECT_SURFDISPLAY_ID && 
            obj->id != OBJECT_BACKGROUND_ID && 
            obj->render_early) {
            // render the object:
            surface_on_draw(
                obj_dsp->surface, 
                obj->surface, 
                (int) obj->scr_pos_x, 
                (int) obj->scr_pos_y);
        }
        
        obj = obj->next_object;
	}
    
    // render loop:
	obj = obj_first;
    
	while (obj != NULL) {
		
		if (obj->id != OBJECT_SURFDISPLAY_ID && 
            obj->id != OBJECT_BACKGROUND_ID && 
            !obj->render_early) {
            
            if (!obj->render_is_in_blobb) {
                
                // render the object:
                surface_on_draw(
                    obj_dsp->surface, 
                    obj->surface, 
                    (int) obj->scr_pos_x, 
                    (int) obj->scr_pos_y);
                    
            } else if (obj->render_blobb != NULL) {
                
                // render all objects in the sorted render blobb:
                listobj_t* blobb = obj->render_blobb;
                
                while (blobb != NULL) {
                    
                    surface_on_draw(
                        obj_dsp->surface, 
                        blobb->obj->surface, 
                        (int) blobb->obj->scr_pos_x, 
                        (int) blobb->obj->scr_pos_y);
                    
                    blobb = blobb->next;
                }
            }
                
		}
        
        // reset render blobbs:
        obj->render_before = listobj_free(obj->render_before);
        obj->render_after = listobj_free(obj->render_after);
        obj->render_blobb = listobj_free(obj->render_blobb);
        obj->render_is_in_blobb = false;
        
        obj = obj->next_object;
	}
    
    // render texts:
	obj = obj_first;
	
	while (obj != NULL) {
		
		if (obj->txt_print != 0) {
			
			on_render_text(obj, obj_dsp);
		}
		
		obj = obj->next_object;
	}
	

	glBindTexture(GL_TEXTURE_2D, obj_dsp->render_id);
	glEnable(GL_TEXTURE_2D);
	
	
	
	// many filters possible with the following code:
	/*uint32_t* pxl = (uint32_t*) surf->pixels;
	uint8_t* pxl8;
	uint8_t pxl_tmp;
	int32_t n;
	for (n = 0; n < surf->w * surf->h; n++) {
		pxl8 = &pxl[n];
		pxl_tmp = pxl8[0];
		pxl8[0] = pxl8[2];
		pxl8[2] = pxl_tmp;
		
		//pxl[n] = pxl[n] << 16;
	}*/
	
	// this reads from the sdl surface and puts it into an opengl 
    // texture
	glTexImage2D(
		GL_TEXTURE_2D, 0, obj_dsp->surface->format->BytesPerPixel, 
		obj_dsp->surface->w, obj_dsp->surface->h, 
		0, 0x80E1, GL_UNSIGNED_BYTE, 
		obj_dsp->surface->pixels);
	
	// clear the color and depth buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// make a rectangle
	glBegin(GL_QUADS);

	// top left
	glTexCoord2i(0, 0);
	glVertex3f(0, 0, 0);

	// top right
	glTexCoord2i(1, 0);
	glVertex3f(400, 0, 0);

	// bottom right
	glTexCoord2i(1, 1);
	glVertex3f(400, 300, 0);

	// bottom left
	glTexCoord2i(0, 1);
	glVertex3f(0, 300, 0);

	glEnd();
 
	glDisable(GL_TEXTURE_2D);
 
	//time = SDL_GetTicks();
 
    SDL_GL_SwapBuffers();
	
	//glDeleteTextures(1, &textureid);
	//printf("time for openGL: %d\n", SDL_GetTicks() - time);

}

// sort the objects in the render list:
void on_render_sort(object_t* obj) {
    
    // construct render blobbs:
    object_t* obj_first = object_get_first(obj);
    
    obj = obj_first;
    while (obj != NULL) {
        
		if (!obj->render_is_in_blobb) {
            
            if (obj->render_before != NULL ||
                obj->render_after != NULL) {
                // add object to its own render blobb list:
                obj->render_blobb = listobj_add(obj->render_blobb, obj);
                obj->render_is_in_blobb = true; // here: has a blobb
            }
            if (obj->render_before != NULL) {
                obj->render_blobb = 
                    render_blobb(obj->render_blobb, obj->render_before);
            }
            if (obj->render_after != NULL) {
                obj->render_blobb = 
                    render_blobb(obj->render_blobb, obj->render_after);
            }
            /*listobj_t* blobb = obj->render_blobb;
            while (blobb != NULL) {
                printf("blobb of obj: %d, obj: %d\n", obj->id, blobb->obj->id);
                blobb = blobb->next;
            }
            printf("\n");*/
            if (obj->render_blobb != NULL) {
                obj->render_blobb = 
                    render_blobb_sort(obj->render_blobb);
            }
            /*blobb = obj->render_blobb;
            while (blobb != NULL) {
                printf("blobb of obj: %d, obj: %d\n", obj->id, blobb->obj->id);
                blobb = blobb->next;
            }
            printf("\n");*/
        }
    
		obj = obj->next_object;
	}
    
}

// collect objects that overlapp and put them into a render "blobb":
listobj_t* render_blobb(listobj_t* blobb, listobj_t* list) {
    
    while (list != NULL) {
        
        object_t* obj = list->obj;
        
        if (!obj->render_is_in_blobb) {
            
            blobb = listobj_add(blobb, obj);
            obj->render_is_in_blobb = true;
            
            if (obj->render_before != NULL) {
                blobb = render_blobb(blobb, obj->render_before);
            }
            if (obj->render_after != NULL) {
                blobb = render_blobb(blobb, obj->render_after);
            }
        }
        list = list->next;
    }
    
    return(blobb);
}

// sort objects inside of a render blobb:
listobj_t* render_blobb_sort(listobj_t* blobb) {
    
    // count number of objects in blobb:
    uint32_t num_objects = listobj_count_objects(blobb);
    
    for (uint32_t n = 0; n <= num_objects; n++) {
        
        bool swapped_places = false;
        
        blobb = render_blobb_sort_iter(blobb, &swapped_places);
        
        if (!swapped_places) { // everything is sorted
            break;
        }
    }
    
    return(blobb);
}

// a single blobb sort iteration:
listobj_t* render_blobb_sort_iter(listobj_t* blobb, bool* swapped) {
    
    listobj_t* first = blobb;
    bool is_first = true;
    listobj_t* previous = NULL;
    
    while (blobb->next != NULL) { // ->next: last entry needs no swap
        
        
        listobj_t* tmp = blobb->next;
        
        object_t* obj = blobb->obj;
        if (listobj_is_member(obj->render_before, blobb->next->obj)) {
            previous = blobb;
            blobb = blobb->next;
        } else {
            // swap places:
            if (is_first) {
                first = blobb->next;
            } else {
                previous->next = tmp;
            }
            blobb->next = blobb->next->next;
            tmp->next = blobb;
            previous = tmp;
            // tell parent function that at least one swap took place:
            *swapped = true;
        }
        
        is_first = false;
    }
    
    return(first);
}

void on_render_text(object_t* obj, object_t* obj_dsp) {
	
	char str[obj->txt->length];
	uint32_t i;
	
	obj->txt_print++;	// frame counter
	
	i = 0;
	while (i < obj->txt_print && i < obj->txt->length) {
		str[i] = obj->txt->str[i];
		i++;
	}
	str[i] = '\0';
	
	if (obj->txt_print < obj->txt->length) {
		if (obj->txt_surface != NULL) {
			SDL_FreeSurface(obj->txt_surface);
		}
		obj->txt_surface = text_print_to_surface(obj->txt, str);
	}
	
	surface_on_draw(
		obj_dsp->surface, obj->txt_surface, 
		(int) obj->scr_pos_x - 50, (int) obj->scr_pos_y - 20);
	
	
	if (obj->txt_print > 220) {
		SDL_FreeSurface(obj->txt_surface);
		obj->txt_surface = NULL;
		obj->txt_print = 0; // disable printing
	}
	
}
