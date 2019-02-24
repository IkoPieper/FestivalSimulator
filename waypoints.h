#ifndef _WAYPOINTS_H_
    #define _WAYPOINTS_H_

#include <SDL/SDL.h> // needed only for NULL
#include <stdbool.h>

typedef struct waypoints waypoints_t;

struct waypoints {
	uint32_t id;
	waypoints_t* prev;             // previous waypoints in list
	waypoints_t* next;             // next waypoints in list
	
	bool active;                   // is activated?
	
	int8_t pos_are_relative;       //  1: positions are relative to the 
	                               //     object position. 
	                               //  0: (default) absolute positions
	                               //     on background sprite
	                               // -1: absolute positions on screen
	// arrays of paths:
	uint32_t num_ways;             // number of entries in arrays
	uint32_t n;                    // current waypoint
	float* pos_x;                  // x position of waypoint (wp)
	float* pos_y;                  // y position of waypoint
	float* pos_x_relative;         // used to save relative x positions
	float* pos_y_relative;         // used to save relative y positions
	float* vel_abs;                // velocity the object should have
	                               //   to get to this wp
	int32_t* frames_max;           // timelimit / frames to get there 
	                               //   0: (default) no limit required
	                               //  -1: immediately
};

waypoints_t* waypoints_init(uint32_t id, uint32_t num_ways);
void waypoints_free(waypoints_t* ways);

#endif
