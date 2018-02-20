#include "festival.h"
 
int on_execute() {
	
    short running = 1;					// program running?
	const int FPS = 60;					// frames per second
    Uint32 time_start, time_end;		// to measure time per frame
    unsigned long frame = 0;			// current frame
	
	object_t* obj = NULL;
	
	// TODO: sollte definitiv in on_init sein. jeweils, wenn das 
	// entsprechende Objekt initialisiert wird. Dafuer muesste aber die
	// object Liste neu gedacht werden.
	obj = object_add(obj, OBJECT_SURFDISPLAY_ID);	// surf display
	obj = object_add(obj, OBJECT_BACKGROUND_ID);	// background
	obj = object_add(obj, OBJECT_HERO_ID);			// hero
	obj = object_add(obj, OBJECT_SCORE_ID);			// score
	
	short* keys = (short*) malloc(256 * sizeof(short));
	int i;
	for (i = 0; i < 256; i++) {
		keys[i] = 0;
	}
		
	load_config(obj); // TODO: sollte in on_init sein oder danach
	
	if(on_init(obj) == 0) {
		on_cleanup(obj);
		return(-1);
	}
	
	// TODO: on_start aufraeumen. Wird vielleicht nichts von benoetigt.
 	on_start(obj);
 	
 	// TODO: on_load_game_state()
 	// objekte, frame, mit gespeicherten informationen 
 	// fuellen
 	
	SDL_Event event;
	//SDL_EnableKeyRepeat(10, 10);
	while (running) {
		
		time_start = SDL_GetTicks();
		
		while (SDL_PollEvent(&event)) {
			
			on_event(&event, keys);
			
			if (keys[SDLK_ESCAPE]) {
				running = 0;
			}
		}

		on_loop(obj, keys, frame);
		on_render(obj);
		
		// ensure constant frame rate:
		time_end = SDL_GetTicks();
		if(1000 / FPS > time_end - time_start) {
			SDL_Delay(1000 / FPS - (time_end - time_start));
		}
		
		frame++;
	}
	
	// TODO: on_saved_game_state()
 	// objekte und frame speichern
	
	// ALWAYS TODO: keep this up to date
	on_cleanup(obj);
 
	return 0;
}
 
int main(int argc, char* argv[]) {
	
	on_execute();
	
	return(1);
}
