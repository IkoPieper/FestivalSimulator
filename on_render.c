#include "on_render.h"
 
void on_render(groups_t* grp, video_t* vid, float dt) {
	
	//Uint32 time;
	object_t* obj_first = grp->obj_first;
    
    on_render_sort(obj_first);
    
    // render background first:
    object_t* obj_bg = grp->obj_bg;
    surface_on_draw(
                vid->surface, 
                obj_bg->surface, 
                (int32_t) obj_bg->scr_pos_x, 
                (int32_t) obj_bg->scr_pos_y);
    
    
    // render early:
    object_t* obj = obj_first;
    
	while (obj != NULL) {
        
        if (!obj->disable_render) {
            
            if (obj != obj_bg && obj->render_early) {
                
                // render the object:
                surface_on_draw(
                    vid->surface, 
                    obj->surface, 
                    (int32_t) obj->scr_pos_x, 
                    (int32_t) obj->scr_pos_y);
            }
        }
        
        obj = obj->next_object;
	}
    
    // render loop:
	obj = obj_first;
    
	while (obj != NULL) {
		
        // check for render_is_in_blobb as well. this allows objects
        // to be rendered that have render_after entry.
        if (!obj->disable_render || obj->render_is_in_blobb) {
        
            if (obj != obj_bg && !obj->render_early) {
                
                if (!obj->render_is_in_blobb) {
                    
                    on_render_object(obj, vid);
                        
                } else if (obj->render_blobb != NULL) {
                    
                    // render all objects in the sorted render blobb:
                    list_t* blobb = obj->render_blobb;
                    
                    while (blobb != NULL) {
                        
                        object_t* obj_tmp = (object_t*) blobb->entry;
                        
                        on_render_object(obj_tmp, vid);
                        
                        blobb = blobb->next;
                    }
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
			
			on_render_meters(obj, vid);
		}
		
		obj = obj->next_object;
	}
    
    on_render_item(grp->obj_hero, vid);
    
    // render texts:
	obj = obj_first;
	
	while (obj != NULL) {
		
		if (obj->txt != NULL && obj->txt_print != 0) {
			
            obj->txt_next_letter += dt;
            if (obj->txt_next_letter > 1.9) {
                obj->txt_next_letter = 0.0;
                obj->txt_print++;	// counter
            }
            on_render_text(obj, vid);
		}
		
		obj = obj->next_object;
	}
    
	glBindTexture(GL_TEXTURE_2D, vid->render_id);
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
		GL_TEXTURE_2D, 0, vid->surface->format->BytesPerPixel, 
		vid->surface->w, vid->surface->h, 
		0, 0x80E1, GL_UNSIGNED_BYTE, 
		vid->surface->pixels);
    
	// clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	
	// make a rectangle
	glBegin(GL_QUADS);

	// top left
	glTexCoord2i(0, 0);
	glVertex3f(0, 0, 0);

	// top right
	glTexCoord2i(1, 0);
	glVertex3f(vid->surface->w, 0, 0);

	// bottom right
	glTexCoord2i(1, 1);
	glVertex3f(vid->surface->w, vid->surface->h, 0);

	// bottom left
	glTexCoord2i(0, 1);
	glVertex3f(0, vid->surface->h, 0);

	glEnd();
 
	glDisable(GL_TEXTURE_2D);
 
	//time = SDL_GetTicks();
	
    SDL_GL_SwapWindow(vid->window);
	
	//glDeleteTextures(1, &vid->render_id);
	//printf("time for openGL: %d\n", SDL_GetTicks() - time);

}

// sort the objects in the render list:
void on_render_sort(object_t* obj) {
    
    // construct render blobbs:
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

// sort objects inside of a render blobb (Kahn's algorithm):
list_t* render_blobb_sort(list_t* blobb) {
    printf("render_blobb_sort()\n");
    
    list_t* blobb_sorted = NULL;
    
    
    list_t* S = NULL;  // all entries without render_after
    list_t* S_first = NULL;
    
    list_t* blobb_first = get_first(blobb); // just to be shure,
                                            // should already be first.
    list_t* blobb_next = NULL;
    
    list_t* ra = NULL;
    
    object_t* obj = NULL;
    
    
    // debug:
    blobb = blobb_first;
    while (blobb != NULL) {
        
        obj = (object_t*) blobb->entry;
        printf("obj->id: %d", obj->id);
        
        list_t* lst = obj->render_after;
        lst = get_first(lst);
        if (lst != NULL) {
            printf(", render after: ");
        }
        while (lst != NULL) {
            
            object_t* obj_rb = (object_t*) lst->entry;
            
            printf("%d, ", obj_rb->id);
            
            lst = lst->next;
        }
        
        printf("\n");
        
        blobb = blobb->next;
    }
    printf("\n");
    
    
    // fill S:
    blobb = blobb_first;
    
    while (blobb != NULL) {
        
        obj = (object_t*) blobb->entry;
        blobb_next = blobb->next;
        
        if (obj->render_after == NULL) {
            
            if (blobb == blobb_first) {
                blobb_first = blobb_first->next;
            }
            take_out(blobb);
            
            insert_after(S, blobb);
            S = blobb;
        }
        
        blobb = blobb_next;
    }
    
    S_first = get_first(S);
    
    // debug:
    printf("objects in S:\n");
    S = S_first;
    while (S != NULL) {
        
        obj = (object_t*) S->entry;
        printf("obj->id: %d\n", obj->id);
        
        S = S->next;
    }
    printf("\n");
    
    printf("remaining objects in blobb:\n");
    blobb = blobb_first;
    while (blobb != NULL) {
        
        obj = (object_t*) blobb->entry;
        printf("obj->id: %d\n", obj->id);
        
        blobb = blobb->next;
    }
    printf("\n");
    
    printf("sorting:\n");
    // sort:
    S = S_first;
    
    while (S_first != NULL) {
        
        // remove an entry from S:
        S = S_first;
        S_first = S_first->next;
        take_out(S);
        
        //debug:
        object_t* obj_tmp = (object_t*) S->entry;
        printf("obj_tmp->id in S: %d\n", obj_tmp->id);
        
        // add to tail of sorted list:
        insert_after(blobb_sorted, S);
        blobb_sorted = S;
        
        blobb = blobb_first;
        
        while (blobb != NULL) {
            
            obj = (object_t*) blobb->entry;
            blobb_next = blobb->next;
            
            // check if S is in the render_after list:
            if ((ra = find(obj->render_after, S->entry)) != NULL) {
                
                printf("found in render_after of obj->id: %d\n", obj->id);
                
                // remove the respective render_after entry. This works 
                // because S is already in blobb_sorted:
                if (obj->render_after == ra) {
                    obj->render_after = obj->render_after->next;
                    // assuming here that the original render_after is 
                    // the first element in the list.
                }
                take_out(ra);
                free(ra);
                
                if (obj->render_after == NULL) {
                    
                    // take out of blobb:
                    if (blobb == blobb_first) {
                        blobb_first = blobb_first->next;
                    }
                    take_out(blobb);
                    
                    // insert into S:
                    insert_before(S_first, blobb);
                    S_first = blobb;
                }
            }
            
            blobb = blobb_next;
        }
    }
    
    // debug:
    printf("\nobjects after sorting:\n");
    blobb = get_first(blobb_sorted);
    while (blobb != NULL) {
        
        obj = (object_t*) blobb->entry;
        printf("obj->id: %d\n", obj->id);
        
        blobb = blobb->next;
    }
    printf("\n");
    
    return(get_first(blobb_sorted));

}

void on_render_object(object_t* obj, video_t* vid) {
    
    // render the object:
    surface_on_draw(
        vid->surface, 
        obj->surface, 
        (int32_t) obj->scr_pos_x, 
        (int32_t) obj->scr_pos_y);
        
    on_render_object_id(vid, obj);
    
    // render item after host. for example: render
    // active water pistol right after hero:
    if (obj->itm != NULL) {
        
        object_t* obj_itm = (object_t*) obj->itm->entry;
        
        if (obj_itm->itm_props != NULL &&
            obj_itm->itm_props->render_after_host) {
        
            surface_on_draw(
                vid->surface, 
                obj_itm->surface, 
                (int32_t) obj_itm->scr_pos_x, 
                (int32_t) obj_itm->scr_pos_y);
                
            on_render_object_id(vid, obj);
        }
    }
}

void on_render_meters(object_t* obj, video_t* vid) {
	
    list_t* lst = obj->mtr;
    
    lst = get_first(lst);
    
    while (lst != NULL) {
        
        meter_t* mtr = (meter_t*) lst->entry;
        
        surface_on_draw(
            vid->surface, mtr->surf, 
            mtr->scr_pos_x, mtr->scr_pos_y);
        
        lst = lst->next;
    }	
}

void on_render_item(object_t* hero, video_t* vid) {
    
    if (hero->itm != NULL) {

        object_t* obj = (object_t*) hero->itm->entry;
        
        // center positon in surface of item meter
        uint32_t w = obj->itm_props->surf->w;
        uint32_t h = obj->itm_props->surf->h;
        list_t* lst = find_id(hero->mtr, METER_ITEM);
        meter_t* mtr = (meter_t*) lst->entry;
        uint32_t x = mtr->scr_pos_x + mtr->surf->w / 2 - w / 2;
        uint32_t y = mtr->scr_pos_y + mtr->surf->h / 2 - h / 2;
        
        surface_on_draw(vid->surface, obj->itm_props->surf, x, y);
    }

}

void on_render_text(object_t* obj, video_t* vid) {
	
    text_t* txt = (text_t*) obj->txt->entry;
    char str[txt->length + 1];
	
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
    int32_t x = (int32_t) obj->scr_pos_x + obj->surface->w;
    int32_t y = (int32_t) obj->scr_pos_y - 10;
    
    int32_t w = obj->txt_surface->w;
    int32_t h = obj->txt_surface->h;
    int32_t w_dsp = vid->surface->w;
    int32_t h_dsp = vid->surface->h;
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
    
	surface_on_draw(vid->surface, obj->txt_surface, x, y);
    
	if (obj->txt_print > txt->duration) {
		SDL_FreeSurface(obj->txt_surface);
		obj->txt_surface = NULL;
		obj->txt_print = 0; // disable printing
	}
	
}

void on_render_object_id(video_t* vid, object_t* obj) {
    
    SDL_Color fg_color = {0, 255, 0};   // text color
 	
    char str[8];
    snprintf(str, 8, "%d", obj->id);
    
    SDL_Surface* surf = TTF_RenderText_Solid(vid->font, str, fg_color);
    
    surface_on_draw(vid->surface, surf, obj->scr_pos_x, obj->scr_pos_y);
    
    SDL_FreeSurface(surf);
}
