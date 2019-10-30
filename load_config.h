#ifndef _LOAD_CONFIG_H_
    #define _LOAD_CONFIG_H_

#include "own_config.h"
#include "object.h"
#include "surface.h"
#include "text.h"
#include <stdlib.h>

configentry* load_config_defaults(configentry* entry, char* path, object_t* obj);
configentry* load_config_animation(configentry* entry, char* path, object_t* obj);
configentry* load_config_waypoints(configentry* entry, char* path, object_t* obj);
configentry* load_config_text(configentry* entry, object_t* obj);

#endif
