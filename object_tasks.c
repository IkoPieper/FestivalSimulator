#include "object.h"

void say(object_t* obj, uint32_t id, uint32_t duration) {
    
    object_select_text(obj, id);
    text_t* txt = (text_t*) obj->txt->entry;
    txt->duration = duration;
    obj->txt_print = 1;
}

bool said(object_t* obj) {
    
    return(obj->txt_print == 0);
}

// if calling say_new, you have to call say_free at a later step:
void say_new(object_t* obj, char* str, uint32_t duration) {
    
    object_add_text(obj, 0);
    text_t* txt = (text_t*) obj->txt->entry;
    text_add_string(txt, str);
    txt->duration = duration;
    obj->txt_print = 1;
}

void say_free(object_t* obj) {
    
    object_remove_text(obj, 0);
}

void face(object_t* obj, object_t* obj_target, float dt) {
    
    float x = obj_target->pos_x;
    float y = obj_target->pos_y;
    object_select_animation_target(obj, x, y);
    object_animate(obj, 0, dt);
}

void stop(object_t* obj) {
    
    if (obj->id == OBJECT_HERO_ID) {
        obj->can_move = false;
    } else {
        if (obj->ways != NULL) {
            waypoints_t* ways = (waypoints_t*) obj->ways->entry;
            ways->active = false;
        }
    }
    //obj->anim_walk = false;
    obj->vel_x = 0.0;
    obj->vel_y = 0.0;
}

void move_on(object_t* obj) {
    
    if (obj->id == OBJECT_HERO_ID) {
        obj->can_move = true;
    } else {
        if (obj->ways != NULL) {
            waypoints_t* ways = obj->ways->entry;
            ways->active = true;
        }
    }
    //obj->anim_walk = true;
}

void drink_beer(object_t* obj, int16_t value) {
    
    list_t* lst = (list_t*) obj->mtr;
    lst = find_id(lst, METER_BEER);
    meter_t* mtr = (meter_t*) lst->entry;
    meter_update(mtr, mtr->value + value);
}

void change_mood(object_t* obj, int16_t value) {
    
    list_t* lst = (list_t*) obj->mtr;
    lst = find_id(lst, METER_MOOD);
    meter_t* mtr = (meter_t*) lst->entry;
    meter_update(mtr, mtr->value + value);
}

void start_waypoints(object_t* obj, uint32_t id) {
    
    object_select_waypoints(obj, id);
    waypoints_t* ways = (waypoints_t*) obj->ways->entry;
    ways->n = 0;
    ways->active = true;
    obj->can_move = true;
    if (count(obj->anim) >= 8) {
        obj->anim_walk = true;
    }
}

bool waypoints_finished(object_t* obj) {
    
    waypoints_t* ways = (waypoints_t*) obj->ways->entry;
    
    return(!ways->active);
}

void hunt_object(object_t* obj, task_t* tsk, bool clockwise, 
    object_t* obj_hunted, float dt) {
    
    if (obj->col != NULL && tsk->counter == 0) {
        
        float vel_x;
        float vel_y;
        collision_t* col = (collision_t*) obj->col->entry;
        uint32_t rdm = rand();
        
        // start to go away from collision in 90° angle:
        if (rdm > RAND_MAX / 4) {
            if (clockwise) {
                vel_x = col->c_y;
                vel_y = -col->c_x;
            } else {
                vel_x = -col->c_y;
                vel_y = col->c_x;
            }
        } else if (rdm > RAND_MAX / 8) {
            if (clockwise) {
                vel_x = -col->c_y;
                vel_y = col->c_x;
            } else {
                vel_x = col->c_y;
                vel_y = -col->c_x;
            }
        } else if (rdm > RAND_MAX / 16) {
            vel_x = -col->c_x;
            vel_y = -col->c_y;
        } else {
            vel_x = col->c_x;
            vel_y = col->c_y;
        }
        
        vel_x *= 2.0;
        vel_y *= 2.0;
        
        obj->vel_x = vel_x;
        obj->vel_y = vel_y;
        obj->disable_damping = true;
        
        tsk->counter = (int32_t) (30.0 / dt);
        
    } else if (tsk->counter > 0) {
        
        // go away from collision:
        tsk->counter--;
        
    } else {
        
        obj->disable_damping = false;
        
        // follow obj_hunted:
        float vel_x = obj_hunted->pos_x - obj->pos_x;
        float vel_y = obj_hunted->pos_y - obj->pos_y;
        
        float norm = sqrtf(vel_x * vel_x + vel_y * vel_y);
        vel_x /= norm;
        vel_y /= norm;
        
        vel_x *= 2.0;
        vel_y *= 2.0;
        
        obj->vel_x = vel_x;
        obj->vel_y = vel_y;
    }
}

bool squared_distance_smaller(
    object_t* obj1, object_t* obj2, float dist_squared) {
    
    float dist_x = obj1->pos_x - obj2->pos_x;
    float dist_y = obj1->pos_y - obj2->pos_y;
    
    return(dist_x * dist_x + dist_y * dist_y < dist_squared);
}

bool squared_distance_greater(
    object_t* obj1, object_t* obj2, float dist_squared) {
    
    float dist_x = obj1->pos_x - obj2->pos_x;
    float dist_y = obj1->pos_y - obj2->pos_y;
    
    return(dist_x * dist_x + dist_y * dist_y > dist_squared);
}

bool (*get_task_function(uint32_t id))(task_t*, object_t*, bool*, 
    uint64_t, float) {
    
    switch (id) {
        case TASK_FIND_BOB: 
            return(&task_find_bob); 
            break;
        case TASK_FIND_EVA: 
            return(&task_find_eva); 
            break;
        case TASK_SECURITY_FENCE: 
            return(&task_security_fence); 
            break;
        case TASK_BUS_PASSENGER: 
            return(&task_bus_passenger); 
            break;
        case TASK_BUS: 
            return(&task_bus); 
            break;
        case TASK_HUNT: 
            return(&task_hunt); 
            break;
    }
    
    return(NULL);
}

bool task_find_bob(
    task_t* tsk, object_t* obj, bool* keys, uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
    
        if (squared_distance_smaller(obj, hero, 20000)) {
        
            stop(obj);
            face(obj, hero, dt);
            say(obj, 1, 150);
            
            tsk->step = 1;
            
            return(true);
        }
    }
    
    if (tsk->step == 1) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
        
        if (keys[KEY_SPACE] && hero->itm != NULL) {
                
            object_t* obj_itm = (object_t*) hero->itm->entry;
            if (obj_itm->itm_props->id == ITEM_MONEY) {
                
                stop(hero);
                face(hero, obj, dt);
                say_new(hero, "Hier! Nimm mein Geld!", 60);
                
                tsk->step = 2;
                
                return(true);
            }
        }
        
        if (said(obj)) {
            
            move_on(obj);
            
            tsk->step = 0;
            
            return(true);
        }
    }
    
    if (tsk->step == 2) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
        
        if (said(hero)) {
            
            say_free(hero);
            
            say(obj, 2, 50);
            
            move_on(obj);
            move_on(hero);
            
            drink_beer(hero, 10);
            change_mood(hero, 10);
            
            tsk->step = 3;
            
            return(true);
        }
    }
    
    if (tsk->step >= 3) {
        
        // use step as counter:
        tsk->step++;
        
        if (tsk->step == 300) {
            tsk->step = 0;
            
            return(true);
        }
        
    }
    
    return(false);
    
}

bool task_find_eva(
    task_t* tsk, object_t* obj, bool* keys, uint64_t frame, float dt) {
    
    return(false);
    
}

bool task_security_fence(task_t* tsk, object_t* obj, bool* keys, 
    uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
    
        if (hero->pos_x < 600.0 && hero->pos_y < 750.0) {
        
            start_waypoints(obj, 1);
            
            tsk->step++;
            
            return(true);
        }
    }
    
    if (tsk->step == 1) {
        
        if (waypoints_finished(obj)) {
            
            say(obj, 1, 150);
            
            stop(obj);
            obj->can_move = false;
            
            tsk->step++;
            
            return(true);
        }
    }
    
    if (tsk->step == 2 || tsk->step == 1) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
        
        if (hero->pos_x > 600.0 || hero->pos_y > 750.0) {
            
            start_waypoints(obj, 2);
            
            tsk->step = 0;
            
            return(true);
        }
        
        if (keys[KEY_SPACE] && hero->itm->id == ITEM_WATER_PISTOL) {
                
            object_t* obj_itm = (object_t*) hero->itm->entry;
            // if hit security:
            if (obj_itm->pos_y < obj->pos_y + obj->surface->h &&
                obj_itm->pos_x > obj->pos_x &&
                obj_itm->pos_x + obj_itm->surface->w 
                    < obj->pos_x + obj->surface->w) {
            
                say(obj, 2, 50);
                
                tsk->step = 3;
                
                return(true);
            }
        }
    }
    
    if (tsk->step == 3) {
        
        if (said(obj)) {
            
            start_waypoints(obj, 3);
            
            tsk->step = 4;
            
            return(true);
        }
    }
    
    if (tsk->step == 4) {
        
        if (waypoints_finished(obj)) {
            
            tsk->step = 5;
            
            return(true);
        }
    }
    
    if (tsk->step == 5) {
        
        object_t* hero = object_get(obj, OBJECT_HERO_ID);
        
        // hunt the hero:
        hero->is_hunted_by_security = true;
        hunt_object(obj, tsk, true, hero, dt);
        
        // reduce heros mood on collision:
        if (obj->col != NULL) {
            if (find_id(obj->col, OBJECT_HERO_ID) != NULL) {
                change_mood(hero, -10);
            }
        }
        
        return(true);
    }
    
    return(false);
}

bool task_bus_passenger(
    task_t* tsk, object_t* obj, bool* keys, uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        
        if (waypoints_finished(obj)) {
            
            // leave area:
            obj->disable_collision = true;
            start_waypoints(obj, 3);
            
            tsk->step = 1;
        }
        
        return(true);
    }
        
    if (tsk->step == 1) {
        
        if (waypoints_finished(obj)) {
            
            // wait for the bus:
            obj->disable_render = true;
            
            tsk->step = 2;
        }
        
        return(true);
    }
    
    if (tsk->step == 2) {
        
        object_t* bus = object_get(obj, OBJECT_BUS);
        task_t* tsk_bus = (task_t*) bus->tsk->entry;
        if (tsk_bus->step == 0 && count(bus->itm) < 4) {
            
            // enter the bus:
            object_add_item(bus, obj, obj->id);
            
            tsk->step = 3;
        }
        
        return(true);
    }
    
    return(false);
}

bool task_bus(
    task_t* tsk, object_t* obj, bool* keys, uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        
        // count passengers:
        if (count(obj->itm) == 4) {
            
            // take them to a nice bus ride:
            
            // take seats in the bus:
            list_t* lst = get_first(obj->itm);
            
            float x_shift = 60;
            while (lst != NULL) {
                object_t* passenger = (object_t*) lst->entry;
                passenger->pos_x = obj->pos_x + x_shift;
                passenger->pos_y = obj->pos_y + 32.0;
                face(passenger, obj, dt);
                passenger->disable_render = false;
                passenger->anim_walk = false;
                x_shift += 60.0;
                lst = lst->next;
            }
            
            start_waypoints(obj, 2);
            
            tsk->step = 1;
        }
        
        return (true);
    }
    
    if (tsk->step == 1) {
        // drive to bus stop:
        
        list_t* lst = get_first(obj->itm);
        
        float x_shift = 70;
        while (lst != NULL) {
            object_t* passenger = (object_t*) lst->entry;
            passenger->pos_x = obj->pos_x + x_shift;
            x_shift += 60.0;
            lst = lst->next;
        }
        
        /*if (obj->pos_x < 1000.0) {
            obj->disable_collision = false;
        }*/
        
        if (waypoints_finished(obj)) {
            
            // walk inside of bus to bus door:
            lst = get_first(obj->itm);
            while (lst != NULL) {
                
                object_t* passenger = (object_t*) lst->entry;
                passenger->anim_walk = true;
                start_waypoints(passenger, 2);
                
                lst = lst->next;
            }
            
            tsk->step = 2;
        }
        
        return(true);
    }
    
    if (tsk->step == 2) {
        
        list_t* lst = get_first(obj->itm);
        while (lst != NULL) {
            
            object_t* passenger = (object_t*) lst->entry;
            if (waypoints_finished(passenger)) {
                // release passenger:
                passenger->disable_collision = false;
                start_waypoints(passenger, 1);
                task_t* tskp = (task_t*) passenger->tsk->entry;
                tskp->step = 0;
                list_t* lst_tmp = lst->next;
                obj->itm = delete_single(lst);
                lst = lst_tmp;
                
            } else {
                lst = lst->next;
            }
        }
        
        // wait until all passengers released:
        if (count(obj->itm) == 0) {
            
            // drive away to the left side:
            start_waypoints(obj, 3);
            
            tsk->step = 3;
        }
        
        return(true);
    }
    
    if (tsk->step == 3) {
        
        // return to the start position:
        /*if (obj->pos_x < 360.0) {
            obj->disable_collision = true;
        }*/
        
        if (waypoints_finished(obj)) {
            
            obj->pos_x = 2000.0;
            obj->pos_y = 1643.0;
            
            tsk->step = 0;
        }
        
        return(true);
    }
    
    
    return(false);
}

bool task_hunt(
    task_t* tsk, object_t* obj, bool* keys, uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        
        // could be optimized by using verlet boxes
        object_t* obj_b = object_get_first(obj);
        while (obj_b != 0) {
            
            if (obj->is_security && obj_b->is_hunted_by_security) {
                
                if (squared_distance_smaller(obj, obj_b, 50000)) {
                    
                    hunt_t* var = (hunt_t*) malloc(sizeof(hunt_t));
                    var->obj_hunted = obj_b;
                    
                    var->clockwise = rand() > RAND_MAX / 2;
                    tsk->variables = (void*) var;
                    
                    stop(obj);
                    
                    tsk->step = 1;
                    break;
                }
            }
            
            obj_b = obj_b->next_object;
        }
        
        return(true);
    }
    
    if (tsk->step == 1) {
        
        object_t* obj_hunted = (object_t*) tsk->variables;
        
        if (obj->is_security && obj_hunted->is_hunted_by_security) {
            
            hunt_t* var = (hunt_t*) tsk->variables;
            hunt_object(obj, tsk, var->clockwise, var->obj_hunted, dt);
            
            // reduce heros mood on collision:
            if (obj->col != NULL) {
                if (find_id(obj->col, OBJECT_HERO_ID) != NULL) {
                    change_mood(object_get(obj, OBJECT_HERO_ID), -10);
                }
            }
            
        } else {
            
            free(tsk->variables);
            tsk->variables = NULL;
            
            tsk->step = 0;
        }
        return(true);
    }
    
    return(false);
}
