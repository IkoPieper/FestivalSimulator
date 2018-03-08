#include "on_render.h"
 
void on_render(object_t* obj) {
	
	Uint32 time;
	
	// draw objects on surf display:
	object_t* obj_dsp = object_get(obj, OBJECT_SURFDISPLAY_ID);
	
	obj = object_get_first_render(obj);
	
	while (obj != NULL) {
		
		if (obj->id != OBJECT_SURFDISPLAY_ID) {
			surface_on_draw(
			obj_dsp->surface, obj->surface, 
			(int) obj->scr_pos_x, (int) obj->scr_pos_y);
		}
		
		obj = obj->next_render;
	}
	

	glBindTexture(GL_TEXTURE_2D, obj_dsp->render_id);
	glEnable(GL_TEXTURE_2D);
	
	GLenum textureFormat;
	
	/* Set the color mode */
	switch (obj_dsp->surface->format->BytesPerPixel) {
		case 4:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				textureFormat = GL_BGRA;
			} else {
				textureFormat = GL_RGBA;
			}
		break;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				textureFormat = GL_BGR;
			} else {
				textureFormat = GL_RGB;
			}
		break;
	}

	// many filters possible with the following code:
	/*unsigned int* pxl = (unsigned int*) surf->pixels;
	Uint8* pxl8;
	Uint8 pxl_tmp;
	int n;
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
		0, textureFormat, GL_UNSIGNED_BYTE, 
		obj_dsp->surface->pixels);
	
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
 
	time = SDL_GetTicks();
 
    SDL_GL_SwapBuffers();
	
	//glDeleteTextures(1, &textureid);
	printf("time for openGL: %d\n", SDL_GetTicks() - time);

}
