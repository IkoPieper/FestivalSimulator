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
                list_t* blobb = obj->render_blobb;
                
                while (blobb != NULL) {
                    
                    object_t* obj_tmp = (object_t*) blobb->entry;
                    
                    surface_on_draw(
                        obj_dsp->surface, 
                        obj_tmp->surface, 
                        (int) obj_tmp->scr_pos_x, 
                        (int) obj_tmp->scr_pos_y);
                    
                    blobb = blobb->next;
                }
            }
                
		}
        
        // reset render blobbs:
        delete_all(obj->render_before);
        obj->render_before = NULL;
        delete_all(obj->render_after);
        obj->render_after = NULL;
        delete_all(obj->render_blobb);
        obj->render_blobb = NULL;
        obj->render_is_in_blobb = false;
        
        obj = obj->next_object;
	}
    
    // render meters:
    obj = obj_first;
	
	while (obj != NULL) {
		
		if (obj->mtr != NULL) {
			
			on_render_meters(obj, obj_dsp);
		}
		
		obj = obj->next_object;
	}
    
    // render texts:
	obj = obj_first;
	
	while (obj != NULL) {
		
		if (obj->txt != NULL && obj->txt_print != 0) {
			
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
                obj->render_blobb = create_before(obj->render_blobb, (void*) obj, 0);
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
            if (obj->render_blobb != NULL) {
                obj->render_blobb = 
                    render_blobb_sort(obj->render_blobb);
            }
        }
    
		obj = obj->next_object;
	}
    
}

// collect objects that overlapp and put them into a render "blobb":
list_t* render_blobb(list_t* blobb, list_t* current) {
    
    while (current != NULL) {
        
        object_t* obj = (object_t*) current->entry;
        
        if (!obj->render_is_in_blobb) {
            
            blobb = create_before(blobb, (void*) obj, 0);
            obj->render_is_in_blobb = true;
            
            if (obj->render_before != NULL) {
                blobb = render_blobb(blobb, obj->render_before);
            }
            if (obj->render_after != NULL) {
                blobb = render_blobb(blobb, obj->render_after);
            }
        }
        current = current->next;
    }
    
    return(blobb);
}

// sort objects inside of a render blobb:
list_t* render_blobb_sort(list_t* blobb) {
    
    bool swapped_places;
    
    // count number of objects in blobb:
    uint32_t num_objects = count(blobb);
    
    for (uint32_t n = 0; n <= num_objects; n++) {
        
        swapped_places = render_blobb_sort_iter(blobb);
        
        if (!swapped_places) { // everything is sorted
            break;
        }
    }
    
    return(blobb);
}

// a single blobb sort iteration:
bool render_blobb_sort_iter(list_t* blobb) {
    
    bool swapped = true;
    
    while (blobb->next != NULL) { // ->next: last entry needs no swap
        
        object_t* obj = (object_t*) blobb->entry;
        
        if (find(obj->render_before, blobb->next->entry) == NULL) { // TODO: maybe render_after and != NULL is faster
            
            // swap places:
            swap(blobb, blobb->next);
            
            // tell parent function that at least one swap took place:
            swapped = true;
        }
        
        blobb = blobb->next;
    }
    
    return(swapped);
}

void on_render_meters(object_t* obj, object_t* obj_dsp) {
	
    list_t* lst = obj->mtr;
    
    lst = get_first(lst);
    
    while (lst != NULL) {
        
        meter_t* mtr = (meter_t*) lst->entry;
        
        surface_on_draw(
            obj_dsp->surface, mtr->surf, 
            mtr->scr_pos_x, mtr->scr_pos_y);
        
        lst = lst->next;
    }
	
}
void on_render_text(object_t* obj, object_t* obj_dsp) {
	
    text_t* txt = (text_t*) obj->txt->entry;
    char str[txt->length + 1];
	
	
	obj->txt_print++;	// frame counter
	
	uint32_t i = 0;
	while (i < obj->txt_print && i < txt->length) {
		str[i] = txt->str[i];
		i++;
	}
	str[i] = '\0';
    
	if (obj->txt_print < txt->length) {
		if (obj->txt_surface != NULL) {
			SDL_FreeSurface(obj->txt_surface);
		}
		obj->txt_surface = text_print_to_surface(txt->font, str, i);
	}
	
    // determine text position. possibly close to object, otherwise at
    // edge of screen:
    int32_t x = (int32_t) obj->scr_pos_x - 40;
    int32_t y = (int32_t) obj->scr_pos_y + 20;
    
    int32_t w = obj->txt_surface->w;
    int32_t h = obj->txt_surface->h;
    int32_t w_dsp = obj_dsp->surface->w;
    int32_t h_dsp = obj_dsp->surface->h;
    if (x < 5) {
        x = 5;
    }
    if (x + w > w_dsp - 5) {
        x = w_dsp - w - 5;
    }
    if (y < 5) {
        y = 5;
    }
    if (y + h > h_dsp - 5) {
        y = h_dsp - h - 5;
    }
    
	surface_on_draw(obj_dsp->surface, obj->txt_surface, x, y);
    
	if (obj->txt_print > txt->duration) {
		SDL_FreeSurface(obj->txt_surface);
		obj->txt_surface = NULL;
		obj->txt_print = 0; // disable printing
	}
	
}
