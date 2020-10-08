#ifndef _LOAD_CONFIG_H_
    #define _LOAD_CONFIG_H_

#include "own_config.h"
#include "tasks.h"

configentry* load_config_defaults(
    configentry* entry, object_t* obj);
    
configentry* load_config_item(
    configentry* entry, object_t* obj);
    
configentry* load_config_animation(
    configentry* entry, object_t* obj, float dt);
    
configentry* load_config_waypoints(
    configentry* entry, object_t* obj, float dt);
    
configentry* load_config_text(configentry* entry, object_t* obj);

#endif
