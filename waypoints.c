#include "waypoints.h"

waypoints_t* waypoints_init(unsigned int id, unsigned int num_ways) {
	
	waypoints_t* ways = NULL;
	
	ways = (waypoints_t*) malloc(sizeof(waypoints_t));
	
	ways->id = id;
	ways->prev = NULL;
	ways->next = NULL;
	
	ways->pos_are_relative = 0;
	ways->num_ways = num_ways;
	ways->n = 0;
	ways->pos_x =      (float*) malloc(num_ways * sizeof(float));
	ways->pos_y =      (float*) malloc(num_ways * sizeof(float));
	ways->pos_x_relative = NULL;
	ways->pos_y_relative = NULL;
	ways->vel_abs =    (float*) malloc(num_ways * sizeof(float));
	ways->frames_max = (int*) malloc(num_ways * sizeof(float));
	
	for (int n = 0; n < num_ways; n++) {
		ways->pos_x[n] = 0.0;
		ways->pos_y[n] = 0.0;
		ways->vel_abs[n] = 0.0;
		ways->frames_max[n] = 0;
	}
	
	return(ways);
}

void waypoints_free(waypoints_t* ways) {
	
	free(ways->pos_x);
	free(ways->pos_y);
	if (ways->pos_x_relative != NULL) {
		free(ways->pos_x_relative);
	}
	if (ways->pos_y_relative != NULL) {
		free(ways->pos_y_relative);
	}
	free(ways->vel_abs);
	free(ways->frames_max);
	
	free(ways);
}
