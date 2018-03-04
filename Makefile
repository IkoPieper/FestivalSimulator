CC=gcc
FLAGS=-lSDL -lSDL_ttf -lm -L/usr/X11R6/lib/ -lGL -lGLU -g -Wall
FILES=festival.c own_config.c object.c animation.c waypoints.c movements.c collisions.c load_config.c on_init.c surface.c font.c event.c on_start.c on_loop.c on_render.c on_cleanup.c


CApp: $(FILES)
	$(CC) -o FestivalSimulator $(FILES) $(FLAGS)
	
	
