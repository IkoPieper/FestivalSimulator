#include "festival.h"
 
bool on_execute() {
	
    bool running = true;				// program running?
	const uint8_t FPS = 60;				// frames per second
    uint32_t time_start, time_end;		// to measure time per frame
    //Uint32 time;						// to measure time for debug
    uint64_t frame = 0;					// current frame
	
	object_t* obj = NULL;
	obj = on_init(obj);
	if(obj == NULL) {
		fprintf(stderr, "Initialization of objects failed!\n");
		return(-1);
	}
	
 	on_start(obj);  // does nothing
 	
 	verletbox_t* vbox = verletbox_init(obj);
 	
 	bool* keys = (bool*) malloc(256 * sizeof(bool));
	for (uint16_t i = 0; i < 256; i++) {
		keys[i] = 0;
	}
 	
	SDL_Event event;
	//SDL_EnableKeyRepeat(10, 10); // needed for windows port?
	while (running) {
		
		time_start = SDL_GetTicks();
		
		while (SDL_PollEvent(&event)) {
			
			on_event(&event, keys);
			
			if (keys[SDLK_ESCAPE] || event.type == SDL_QUIT) {
				running = 0;
			}
		}

		//time = SDL_GetTicks();
		on_loop(obj, vbox, keys, frame);
		//printf("time for on_loop: %d\n", SDL_GetTicks() - time);
		//time = SDL_GetTicks();
		on_render(obj);
		//printf("time for on_render: %d\n", SDL_GetTicks() - time);
		
		// ensure constant frame rate:
		time_end = SDL_GetTicks();
		if(1000 / FPS > time_end - time_start) {
			SDL_Delay(1000 / FPS - (time_end - time_start));
		}
		
		frame++;
	}
	
	// ALWAYS TODO: keep this up to date
	on_cleanup(obj, vbox, keys);
 
	return false;
}
 
int main(int argc, char* argv[]) {
	
	on_execute();
	
	return 0;
}
