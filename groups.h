// groups are used to improve performance. for example: instead of 
// iterating all objects and check if the object is a member of the 
// festival security, iterate over the securities list.

#ifndef _GROUPS_H_
    #define _GROUPS_H_

#include "object.h"

groups_t* groups_init(object_t* obj);
void groups_free(groups_t* grp);

#endif
