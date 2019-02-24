#include "on_render.h"
 
void on_render(object_t* obj) {
	
	//Uint32 time;
	
	object_t* obj_first = object_get_first_render(obj);
	
	// draw objects on surf display:
	object_t* obj_dsp = object_get(obj, OBJECT_SURFDISPLAY_ID);
	
	obj = obj_first;
	
	while (obj != NULL) {
		
		if (obj->id != OBJECT_SURFDISPLAY_ID) {
			surface_on_draw(
			obj_dsp->surface, obj->surface, 
			(int) obj->scr_pos_x, (int) obj->scr_pos_y);
		}
		
		obj = obj->next_render;
	}
	
	obj = obj_first;
	
	while (obj != NULL) {
		
		if (obj->txt_print != 0) {
			
			on_render_text(obj, obj_dsp);
		}
		
		obj = obj->next_render;
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
	
	// this reads from the sdl surface and puts it into an opengl texture
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
