#include "tasks.h"

void tasks_add_to_object(object_t* obj, uint32_t id) {
    
    task_t* tsk = (task_t*) malloc(sizeof(task_t));
    tsk->step = 0;
    tsk->counter = 0;
    tsk->variables = NULL;
    tasks_get_functions(tsk, id);
    obj->tsk = create_before(obj->tsk, tsk, id);
}

void tasks_free(groups_t* grp) {
    
    object_t* obj;
    
    list_t* lst = grp->lst_have_tsk;
    
    while (lst != NULL) {
    
        obj = (object_t*) lst->entry;
    
        list_t* lst_tsk = get_first(obj->tsk);
        list_t* tmp = lst_tsk;
        
        while (lst_tsk != NULL) {
            
            task_t* tsk = (task_t*) lst_tsk->entry;
            
            /*if (tsk->task_function == &task_hunt || 
                tsk->task_function == &task_soccer) {
                
                free((hunt_t*) tsk->variables);
            }*/
            if (tsk->task_function_free != NULL) {
                tsk->task_function_free(tsk, obj, grp, NULL, 0, 0.0);
            }
            
            free(tsk);
            lst_tsk = lst_tsk->next;
        }
        
        delete_all(tmp);
        
        lst = lst->next;
    }
}

void tasks_get_functions(task_t* tsk, uint32_t id) {
    
    switch (id) {
        case TASK_FIND_BOB:
            tsk->task_function_init = NULL;
            tsk->task_function = &task_find_bob;
            tsk->task_function_free = NULL;
            break;
        case TASK_FIND_EVA:
            tsk->task_function_init = NULL;
            tsk->task_function = &task_find_eva;
            tsk->task_function_free = NULL;
            break;
        case TASK_SECURITY_FENCE:
            tsk->task_function_init = NULL;
            tsk->task_function = &task_security_fence;
            tsk->task_function_free = NULL;
            break;
        case TASK_BUS_PASSENGER:
            tsk->task_function_init = NULL;
            tsk->task_function = &task_bus_passenger;
            tsk->task_function_free = NULL;
            break;
        case TASK_BUS:
            tsk->task_function_init = NULL;
            tsk->task_function = &task_bus;
            tsk->task_function_free = NULL;
            break;
        case TASK_HUNT:
            tsk->task_function_init = NULL;
            tsk->task_function = &task_hunt;
            tsk->task_function_free = &task_hunt_free;
            break;
        case TASK_SOCCER:
            tsk->task_function_init = NULL;
            tsk->task_function = &task_soccer;
            tsk->task_function_free = &task_soccer_free;
            break;
        case TASK_SOCCER_BALL:
            tsk->task_function_init = NULL;
            tsk->task_function = &task_soccer_ball;
            tsk->task_function_free = NULL;
            break;
        case TASK_FLUNKY_0:
            tsk->task_function_init = NULL;
            tsk->task_function = &task_flunky;
            tsk->task_function_free = &task_flunky_free;
            break;
        case TASK_FLUNKY_1:
            tsk->task_function_init = NULL;
            tsk->task_function = &task_flunky;
            tsk->task_function_free = &task_flunky_free;
            break;
        default:
            tsk->task_function_init = NULL;
            tsk->task_function = NULL;
            tsk->task_function_free = NULL;
    }
}

void task_find_bob(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        
        object_t* hero = grp->obj_hero;
    
        if (squared_distance_smaller(obj, hero, 20000)) {
        
            stop(obj);
            face(obj, hero, dt);
            say(obj, 1, 150);
            
            tsk->step = 1;
        }
        
    } else if (tsk->step == 1) {
        
        object_t* hero = grp->obj_hero;
        
        if (keys[KEY_SPACE] && hero->itm != NULL) {
                
            object_t* obj_itm = (object_t*) hero->itm->entry;
            if (obj_itm->itm_props->id == ITEM_MONEY) {
                
                stop(hero);
                face(hero, obj, dt);
                say_new(hero, "Hier! Nimm mein Geld!", 60);
                
                tsk->step = 2;
                
                return;
            }
        }
        
        if (said(obj)) {
            
            move_on(obj);
            
            tsk->step = 0;
        }
        
    } else if (tsk->step == 2) {
        
        object_t* hero = grp->obj_hero;
        
        if (said(hero)) {
            
            say_free(hero);
            
            say(obj, 2, 50);
            
            move_on(obj);
            move_on(hero);
            
            drink_beer(hero, 10);
            change_mood(hero, 10);
            
            tsk->step = 3;
        }
        
    } else if (tsk->step >= 3) {
        
        // use step as counter:
        tsk->step++;
        
        if (tsk->step == 300) {
            
            tsk->step = 0;
        }
    }
}

void task_find_eva(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
}

void task_security_fence(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        
        object_t* hero = grp->obj_hero;
    
        if (hero->pos_x < 600.0 && hero->pos_y < 750.0) {
        
            start_waypoints(obj, 1);
            
            tsk->step = 1;
            
            return;
        }
    }
    
    if (tsk->step == 1) {
        
        if (waypoints_finished(obj)) {
            
            say(obj, 1, 150);
            
            stop(obj);
            obj->can_move = false;
            
            tsk->step = 2;
            
            return;
        }
    }
    
    if (tsk->step == 2 || tsk->step == 1) {
        
        object_t* hero = grp->obj_hero;
        
        if (hero->pos_x > 600.0 || hero->pos_y > 750.0) {
            
            start_waypoints(obj, 2);
            
            tsk->step = 0;
            
            return;
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
                
                return;
            }
        }
    }
    
    if (tsk->step == 3) {
        
        if (said(obj)) {
            
            start_waypoints(obj, 3);
            
            tsk->step = 4;
            
            return;
        }
    }
    
    if (tsk->step == 4) {
        
        if (waypoints_finished(obj)) {
            
            tsk->step = 5;
            
            return;
        }
    }
    
    if (tsk->step == 5) {
        
        object_t* hero = grp->obj_hero;
        
        // hunt the hero:
        hero->is_hunted_by_security = true;
        hunt_object(obj, tsk, true, hero, dt);
        
        // reduce heros mood on collision:
        if (obj->col != NULL) {
            if (find_id(obj->col, OBJECT_HERO_ID) != NULL) {
                change_mood(hero, -10);
            }
        }
        
        return;
    }
}

void task_bus_passenger(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        
        if (waypoints_finished(obj)) {
            
            // leave area:
            obj->disable_collision = true;
            start_waypoints(obj, 3);
            
            tsk->step = 1;
        }
        
    } else if (tsk->step == 1) {
        
        if (waypoints_finished(obj)) {
            
            // wait for the bus:
            obj->disable_render = true;
            
            tsk->step = 2;
        }
        
    } else if (tsk->step == 2) {
        
        object_t* bus = object_get(obj, OBJECT_BUS);
        task_t* tsk_bus = (task_t*) bus->tsk->entry;
        if (tsk_bus->step == 0 && count(bus->itm) < 4) {
            
            // enter the bus:
            items_add_to_object(bus, obj, obj->id);
            
            tsk->step = 3;
        }
    }
}

void task_bus(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
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
                object_select_animation(passenger, ANIMATION_REST_WEST);
                object_animate(passenger, 0, dt);
                passenger->disable_render = false;
                passenger->anim_walk = false;
                x_shift += 60.0;
                lst = lst->next;
            }
            
            start_waypoints(obj, 2);
            
            tsk->step = 1;
        }
        
    } else if (tsk->step == 1) {
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
        
    } else if (tsk->step == 2) {
        
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
        
    } else if (tsk->step == 3) {
        
        // return to the start position:
        /*if (obj->pos_x < 360.0) {
            obj->disable_collision = true;
        }*/
        
        if (waypoints_finished(obj)) {
            
            obj->pos_x = 2000.0;
            obj->pos_y = 1643.0;
            
            tsk->step = 0;
        }
    }
}

void task_hunt(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        
        // could be optimized by using verlet boxes
        object_t* obj_b = grp->obj_first;
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
        
    } else if (tsk->step == 1) {
        
        object_t* obj_hunted = (object_t*) tsk->variables;
        
        if (obj->is_security && obj_hunted->is_hunted_by_security) {
            
            hunt_t* var = (hunt_t*) tsk->variables;
            hunt_object(obj, tsk, var->clockwise, var->obj_hunted, dt);
            
            // reduce heros mood on collision:
            if (obj->col != NULL) {
                if (find_id(obj->col, OBJECT_HERO_ID) != NULL) {
                    change_mood(grp->obj_hero, -10);
                }
            }
            
        } else {
            
            free(tsk->variables);
            tsk->variables = NULL;
            
            tsk->step = 0;
        }
    }
}

/*void task_hunt_save( //unfinished
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    hunt_t* var = (hunt_t*) tsk->variables;
    
    fprintf(fid, "obj_hunted = %d\n", var->obj_hunted->id);
    fprintf(fid, "clockwise = %d\n", var->clockwise);
}

void task_hunt_load( //unfinished
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
}*/

void task_hunt_free(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    free((hunt_t*) tsk->variables);
}

void hunt_object(
    object_t* obj, task_t* tsk, bool clockwise, 
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
        float vel_x = (obj_hunted->pos_x + obj_hunted->wall->x) - 
            (obj->pos_x + obj->wall->x);
        float vel_y = (obj_hunted->pos_y + obj_hunted->wall->y) - 
            (obj->pos_y + obj->wall->y);
        
        float norm = sqrtf(vel_x * vel_x + vel_y * vel_y);
        vel_x /= norm;
        vel_y /= norm;
        
        vel_x *= 2.0;
        vel_y *= 2.0;
        
        obj->vel_x = vel_x;
        obj->vel_y = vel_y;
    }
}

void task_soccer(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        
        object_t* ball = object_get(grp->obj_first, 999);
        
        hunt_t* var = (hunt_t*) malloc(sizeof(hunt_t));
        var->obj_hunted = ball;
        var->clockwise = rand() > RAND_MAX / 2;
        tsk->variables = (void*) var;
        
        tsk->step = 1;
        
        hunt_object(obj, tsk, true, ball, dt);
        
    } else if (tsk->step == 1) {
        
        hunt_t* var = (hunt_t*) tsk->variables;
        hunt_object(obj, tsk, var->clockwise, var->obj_hunted, dt);
        
        if (obj->pos_x > 925.0 && obj->vel_x > 0) {
            
            obj->vel_x = -obj->vel_x;
        }
    }
}

void task_soccer_free(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    free((hunt_t*) tsk->variables);
}

void task_soccer_ball(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    if (obj->pos_x > 925.0 && obj->vel_x > 0) {
        
        obj->vel_x = -obj->vel_x;
    }
}

void task_flunky(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    
    /*if (tsk->step == 0) {
        
        
        // init the object's (player) task variables:
        printf("init task flunky for object %d!\n", obj->id);
        
        flunky_t* var = (flunky_t*) malloc(sizeof(flunky_t));
        var->global = NULL;
        var->pos_x_line = (uint32_t) obj->pos_x;  // start position behind line
        var->pos_y_line = (uint32_t) obj->pos_y;  // start position behind line
        if (abs(var->pos_y_line - pos_y_line_team_a) <
            abs(var->pos_y_line - pos_y_line_team_b)) {
            
            var->team_b = false;                // obj (player) is in team a
            obj->facing = OBJECT_FACING_SOUTH;
            printf("object %d joined team a!\n", obj->id);
        } else {
            var->team_b = true;                 // obj (player) is in team b
            obj->facing = OBJECT_FACING_NORTH;
            printf("object %d joined team b!\n", obj->id);
        }
        tsk->variables = (void*) var;
        
        if (!static_variables_initialized) { // only call for first player
            
            // init the global variables of the whole flunky ball game:
            var->global = (flunky_global_t*) malloc(sizeof(flunky_global_t));
            
            var->global->pos_y_line_team_a = 1070;
            var->global->pos_y_line_team_b = 1240;
            var->global->ball = object_get(obj, 899);    // get ball object
            
            const uint8_t max_num_player = 8;
            for (uint8_t i = 0; i < max_num_player; i++) {
                var->global->team_a[i] = NULL;
                var->global->team_b[i] = NULL;
            }
            var->global->num_player_team_a = 0;
            var->global->num_player_team_b = 0;
            
            global_variables_initialized = true;
            
        } else { // other players
            
            // get pointer to global game variables:
            
        }
        
        if (var->team_b) {
            team_b[num_player_team_b++] = obj;  // add obj to team b array
        } else {
            team_a[num_player_team_a++] = obj;  // add obj to team a array
        }
        
        tsk->step = 1;
        
    } else if (tsk->step == 1) {
        
        static_variables_initialized = false;   // init again, the next time 
        
        for (uint8_t i = 0; i < num_player_team_a; i++) {
            printf("object %d is in team a!\n", team_a[i]->id);
        }
        for (uint8_t i = 0; i < num_player_team_b; i++) {
            printf("object %d is in team b!\n", team_b[i]->id);
        }
        
        tsk->step = 2;
    }*/
}
    
void task_flunky_free(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    
    
    free((flunky_t*) tsk->variables);
}
