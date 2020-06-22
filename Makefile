CC=gcc -std=c99
FLAGS=-lSDL2 -lSDL2_mixer -lSDL2_ttf -lm -L/usr/X11R6/lib/ -lGL -lGLU -g -Wall
FILES=festival.c own_config.c tasks.c items.c groups.c object.c list.c meter.c sound.c animation.c text.c waypoints.c movements.c collisions.c verletbox.c load_config.c on_init.c surface.c event.c on_start.c on_loop.c on_render.c on_cleanup.c


CApp: $(FILES)
	$(CC) -o FestivalSimulator $(FILES) $(FLAGS)
	
	
