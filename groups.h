// groups are used to improve performance. for example: instead of 
// iterating all objects and check if the object is a member of the 
// festival security, iterate over the securities list.

#ifndef _GROUPS_H_
    #define _GROUPS_H_

#include "list.h"
#include "object.h"

typedef struct groups groups_t;

struct groups {
	list_t* lst_securities; // objects that are securities
	list_t* lst_dixis;      // objects that are dixis
    list_t* lst_have_anim;  // objects that have animations
    list_t* lst_have_ways;  // objects that have waypoints
    list_t* lst_have_tsk;   // objects that have tasks
    object_t* obj_bg;       // the object that acts as background
    object_t* obj_hero;     // the hero of the game
    object_t* obj_first;    // first object in object list
};

groups_t* groups_init(object_t* obj);
void groups_free(groups_t* grp);

#endif
