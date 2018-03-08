#include "festival.h"
 
int on_execute() {
	
    short running = 1;					// program running?
	const int FPS = 60;					// frames per second
    Uint32 time_start, time_end;		// to measure time per frame
    Uint32 time;						// to measure time for debug
    unsigned long frame = 0;			// current frame
	
	
	//load_config(obj); // TODO: sollte in on_init sein oder danach
	
	object_t* obj = on_init(obj);
	if(obj == NULL) {
		fprintf(stderr, "Initialization of objects failed!\n");
		return(-1);
	}
	
 	on_start(obj);  // does nothing
 	
 	// TODO: on_load_game_state()
 	// objekte, frame, mit gespeicherten informationen 
 	// fuellen
 	
 	
 	verletbox_t* vbox = verletbox_init(obj);
 	
 	printf("\n");
 	fprintf(stderr, "vbox->num_w: %d, vbox->num_h: %d\n", vbox->num_w, vbox->num_h);
 	for (int x = 0; x < vbox->num_w; x++) {
		for (int y = 0; y < vbox->num_h; y++) {
			object_t* obj_tmp = vbox->boxes[x][y];
			while (obj_tmp != NULL) {
				printf("%3d,", obj_tmp->id);
				obj_tmp = obj_tmp->next_vbox;
			}
			printf(":");
		}
		printf("\n");
	}
 	printf("\n");
 	
 	short* keys = (short*) malloc(256 * sizeof(short));
	int i;
	for (i = 0; i < 256; i++) {
		keys[i] = 0;
	}
 	
	SDL_Event event;
	//SDL_EnableKeyRepeat(10, 10); // needed for windows port?
	while (running) {
		
		time_start = SDL_GetTicks();
		
		while (SDL_PollEvent(&event)) {
			
			on_event(&event, keys);
			
			if (keys[SDLK_ESCAPE]) {
				running = 0;
			}
		}

		time = SDL_GetTicks();
		on_loop(obj, vbox, keys, frame);
		printf("time for on_loop: %d\n", SDL_GetTicks() - time);
		time = SDL_GetTicks();
		on_render(obj);
		printf("time for on_render: %d\n", SDL_GetTicks() - time);
		
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
