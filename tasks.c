#include "tasks.h"

void tasks_add_to_object(object_t* obj, uint32_t id) {
    
    task_t* tsk = (task_t*) malloc(sizeof(task_t));
    tsk->id = id;
    tsk->step = 0;
    tsk->counter = 0;
    tsk->variables = NULL;
    tsk->variables_shared = NULL;
    tsk->task_function = NULL;
    tsk->task_function_free = NULL;
    tasks_get_functions(tsk, id);
    obj->tsk = create_before(obj->tsk, tsk, id);
}

void tasks_init(groups_t* grp) {
    
    if (grp->lst_have_tsk_flunky_0 != NULL) {
        task_flunky_init(grp->lst_have_tsk_flunky_0, TASK_FLUNKY_0);
    }
    if (grp->lst_have_tsk_flunky_1 != NULL) {
        task_flunky_init(grp->lst_have_tsk_flunky_1, TASK_FLUNKY_1);
    }
    
}

void tasks_free(groups_t* grp) {
    
    bool freed_flunky_0 = false;
    bool freed_flunky_1 = false;
    
    object_t* obj;
    
    list_t* lst = grp->lst_have_tsk;
    
    // loop over objects which have tasks:
    while (lst != NULL) {
    
        obj = (object_t*) lst->entry;
    
        list_t* lst_tsk = get_first(obj->tsk);
        list_t* tmp = lst_tsk;
        
        // loop over tasks of object:
        while (lst_tsk != NULL) {
            
            task_t* tsk = (task_t*) lst_tsk->entry;
            
            if (tsk->task_function_free != NULL) {
                tsk->task_function_free(tsk, obj, grp, NULL, 0, 0.0);
            }
            
            if (tsk->variables_shared != NULL) {
                switch (tsk->id) {
                    case TASK_FLUNKY_0:
                        if (!freed_flunky_0) {
                            task_flunky_free_shared(tsk->variables_shared, 0);
                            freed_flunky_0 = true;
                        }
                        break;
                    case TASK_FLUNKY_1:
                        if (!freed_flunky_1) {
                            task_flunky_free_shared(tsk->variables_shared, 1);
                            freed_flunky_1 = true;
                        }
                        break;
                }
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
            tsk->task_function = &task_find_bob;
            break;
        case TASK_FIND_EVA:
            tsk->task_function = &task_find_eva;
            break;
        case TASK_SECURITY_FENCE:
            tsk->task_function = &task_security_fence;
            break;
        case TASK_BUS_PASSENGER:
            tsk->task_function = &task_bus_passenger;
            break;
        case TASK_BUS:
            tsk->task_function = &task_bus;
            break;
        case TASK_HUNT:
            tsk->task_function = &task_hunt;
            tsk->task_function_free = &task_hunt_free;
            break;
        case TASK_SOCCER:
            tsk->task_function = &task_soccer;
            tsk->task_function_free = &task_soccer_free;
            break;
        case TASK_SOCCER_BALL:
            tsk->task_function = &task_soccer_ball;
            break;
        case TASK_FLUNKY_0:
            tsk->task_function = &task_flunky;
            tsk->task_function_free = &task_flunky_free;
            break;
        case TASK_FLUNKY_1:
            tsk->task_function = &task_flunky;
            tsk->task_function_free = &task_flunky_free;
            break;
        default:
            tsk->task_function = NULL;
            tsk->task_function_free = NULL;
    }
}

// get the task of object based on task id:
task_t* tasks_get_task(object_t* obj, uint32_t id) {
        
    list_t* lst_tsk = find_id(obj->tsk, id);
    return ((task_t*) lst_tsk->entry);
}

// write pointer to shared task variables into the task of every object in 
// lst_obj:
void tasks_share_variables(list_t* lst_obj, void* var_shared, uint32_t id) {
    
    lst_obj = get_first(lst_obj);
    
    while (lst_obj != NULL) {
        
        object_t* obj = (object_t*) lst_obj->entry;
        
        task_t* tsk = tasks_get_task(obj, id);
        
        tsk->variables_shared = var_shared;
        
        lst_obj = lst_obj->next;
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
        hunt_object(obj, &tsk->counter, true, hero, dt);
        
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
        
        hunt_t* var = (hunt_t*) tsk->variables;
        
        if (obj->is_security && var->obj_hunted->is_hunted_by_security) {
            
            hunt_object(obj, &tsk->counter, var->clockwise, var->obj_hunted, dt);
            
            // reduce heros mood on collision:
            if (var->obj_hunted->id == OBJECT_HERO_ID) {
                if (check_collision(obj, OBJECT_HERO_ID)) {
                    change_mood(grp->obj_hero, -10);
                }
            }
            /*if (obj->col != NULL && var->obj_hunted->id == OBJECT_HERO_ID) {
                if (find_id(obj->col, OBJECT_HERO_ID) != NULL) {
                    change_mood(grp->obj_hero, -10);
                }
            }*/
            
        } else {
            
            free((hunt_t*) tsk->variables);
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
    
    if (tsk->variables != NULL) {
        free((hunt_t*) tsk->variables);
    }
}

void hunt_object(
    object_t* obj, uint32_t* counter, bool clockwise, 
    object_t* obj_hunted, float dt) {
    
    const float vel_abs = 2.0;
    
    if (obj->col != NULL && *counter == 0) {
        
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
        
        vel_x *= vel_abs;
        vel_y *= vel_abs;
        
        obj->vel_x = vel_x;
        obj->vel_y = vel_y;
        obj->disable_damping = true;
        
        *counter = (int32_t) (30.0 / dt);
        
    } else if (*counter > 0) {
        
        // go away from collision:
        (*counter)--;
        
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
        
        vel_x *= vel_abs;
        vel_y *= vel_abs;
        
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
        
        hunt_object(obj, &tsk->counter, true, ball, dt);
        
    } else if (tsk->step == 1) {
        
        hunt_t* var = (hunt_t*) tsk->variables;
        hunt_object(obj, &tsk->counter, var->clockwise, var->obj_hunted, dt);
        
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

void task_flunky_init(list_t* lst_obj, uint32_t id) {
    
    flunky_shared_t* var_shared = 
        (flunky_shared_t*) malloc(sizeof(flunky_shared_t));
    
    tasks_share_variables(lst_obj, (void*) var_shared, id);
    
    uint32_t pos_y_line_team_a;
    uint32_t pos_y_line_team_b;
    
    switch (id) {
        case TASK_FLUNKY_0:
            var_shared->target = object_get((object_t*) lst_obj->entry, 898);
            var_shared->ball = object_get((object_t*) lst_obj->entry, 899);
            pos_y_line_team_a = 1070;
            pos_y_line_team_b = 1240;
            var_shared->pos_y_line_team_a = pos_y_line_team_a;
            var_shared->pos_y_line_team_b = pos_y_line_team_b;
            break;
        case TASK_FLUNKY_1:
            // change values if adding TASK_FLUNKY_1 game:
            var_shared->target = object_get((object_t*) lst_obj->entry, 898);
            var_shared->ball = object_get((object_t*) lst_obj->entry, 899);
            pos_y_line_team_a = 1070;
            pos_y_line_team_b = 1240;
            var_shared->pos_y_line_team_a = pos_y_line_team_a;
            var_shared->pos_y_line_team_b = pos_y_line_team_b;
            break;
    }
    
    // put the players (obj) into team arrays. also init the player task 
    // variables here as well. this is usually done later, in the first step 
    // inside the players task function, but here is nicer:
    
    var_shared->target_hit = false;
    var_shared->ball_retrieved = true;
    var_shared->target_retrieved = true;
    var_shared->num_finished = 0;
    
    var_shared->team_a_turn = true;
    var_shared->team_b_turn = false;
    
    var_shared->num_player_team_a = 4;
    var_shared->num_player_team_b = 4;
    
    
    // malloc team pointer arrays:
    var_shared->team_a = 
        (object_t**) malloc(var_shared->num_player_team_a * sizeof(object_t*));
    var_shared->team_b = 
        (object_t**) malloc(var_shared->num_player_team_b * sizeof(object_t*));
    
    uint8_t a = 0;  // team a counter
    uint8_t b = 0;  // team b counter
    
    object_t* obj;
    lst_obj = get_first(lst_obj);
    
    while (lst_obj != NULL) {
        
        // get object:
        obj = (object_t*) lst_obj->entry;
        
        task_t* tsk = tasks_get_task(obj, id);
        
        // malloc task variables of object:
        tsk->variables = (void*) malloc(sizeof(flunky_t));
        flunky_t* var = (flunky_t*) tsk->variables;
        
        var->pos_x_default = obj->pos_x;
        var->pos_y_default = obj->pos_y;
        var->step = 0;
        var->counter = 0;
        var->clockwise = false;
        
        // init task variables of object:
        switch (obj->id) {
            case 801 ... 804:   // obj (player) is in team a
                var->is_player = true;
                var->is_beer = false;
                var->is_ball = false;
                var->is_target = false;
                var->team_a = true;
                var->team_b = false;
                var->beer = 
                    object_get((object_t*) lst_obj->entry, obj->id + 10);
                //obj->facing = OBJECT_FACING_SOUTH;
                obj->facing = OBJECT_FACING_SOUTH;
                printf("object %d joined team a!\n", obj->id);
                var_shared->team_a[a++] = obj;
                break;
            
            case 805 ... 808:   // obj (player) is in team b
                var->is_player = true;
                var->is_beer = false;
                var->is_ball = false;
                var->is_target = false;
                var->team_a = false;
                var->team_b = true;
                var->beer = 
                    object_get((object_t*) lst_obj->entry, obj->id + 10);
                obj->facing = OBJECT_FACING_NORTH;
                printf("object %d joined team b!\n", obj->id);
                var_shared->team_b[b++] = obj;
                break;
                
            case 811 ... 814:   // beer bottles of team a
                var->is_player = false;
                var->is_beer = true;
                var->is_ball = false;
                var->is_target = false;
                var->team_a = true;
                var->team_b = false;
                break;
                
            case 815 ... 818:   // beer bottles of team b
                var->is_player = false;
                var->is_beer = true;
                var->is_ball = false;
                var->is_target = false;
                var->team_a = false;
                var->team_b = true;
                break;
                
            case 898:           // flunky target
                var->is_player = false;
                var->is_beer = false;
                var->is_ball = false;
                var->is_target = true;
                var->team_a = false;
                var->team_b = false;
                var_shared->pos_x_target = obj->pos_x;
                var_shared->pos_y_target = obj->pos_y;
                obj->disable_collision = true;
                break;
                
            case 899:           // flunky ball
                var->is_player = false;
                var->is_beer = false;
                var->is_ball = true;
                var->is_target = false;
                var->team_a = false;
                var->team_b = false;
                break;
        }
        
        // give ball to a player of team a:
        if (obj->id == 803) {
            pick_up(obj, var_shared->ball);
        }
        
        lst_obj = lst_obj->next;
    }
    
}

void task_flunky_free_shared(void* var_shared, uint32_t id) {
    
    flunky_shared_t* var = (flunky_shared_t*) var_shared;
    
    free(var->team_a);
    free(var->team_b);
    free(var);
}

void task_flunky(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    flunky_t* var = (flunky_t*) tsk->variables;
    flunky_shared_t* var_shared = (flunky_shared_t*) tsk->variables_shared;
    
    if (var->is_player) {
        
        task_flunky_player(obj, var, var_shared, frame, dt);
        
    } else if (var->is_beer) {
        
        task_flunky_beer(obj, var, var_shared, frame, dt);
        
    } else if (var->is_target) {
        
        task_flunky_target(obj, var, var_shared, frame, dt);
        
    } else if (var->is_ball) {
        
        task_flunky_ball(obj, var, var_shared, frame, dt);
    }
}

void task_flunky_free(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    free((flunky_t*) tsk->variables);
}

void task_flunky_player(
    object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
    bool players_turn = 
        (var->team_a && var_shared->team_a_turn) ||
        (var->team_b && var_shared->team_b_turn);
    
    if (var_shared->team_b_turn) {
        frame++;
    }
        
    if (players_turn) {
        
        if (var->step == 666) {
            var->step = 0;
        }
        
        if (var_shared->target_hit) {
            
            task_flunky_player_drink(obj, var, var_shared, frame, dt);
            
        } else {
            
            if (obj->obj_carries == var_shared->ball) {
                task_flunky_player_throw_ball(obj, var, var_shared, frame, dt);
            }
        }
        
    } else {    // other teams turn
        
        if (obj->obj_carries == var->beer) {
            throw(obj, 0.0);
        }
        
        if (var_shared->target_hit) {
            
            task_flunky_player_retrieve_ball_or_target(
                obj, var, var_shared, frame, dt);
        }
    }
}

void task_flunky_beer(
    object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
}

void task_flunky_target(
    object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
    /*const float x_border = 4.0;
    const float y_border = 4.0;
    float x = obj->pos_x;
    float y = obj->pos_y;
    float x_def = var->pos_x_default;
    float y_def = var->pos_y_default;
    
    if (x < x_def - x_border ||
        x > x_def + x_border ||
        y < y_def - y_border ||
        y > y_def + y_border) {
        
        var_shared->target_hit = true;
    }*/
    if (check_collision(obj, var_shared->ball->id)) {
        var_shared->target_hit = true;
    }
}

void task_flunky_ball(
    object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
    
}

void task_flunky_player_drink(
    object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
    pick_up(obj, var->beer);
}

void task_flunky_player_throw_ball(
    object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
    const float vel_abs = 4.0;
    static float x = 0.0;
    static float y = 0.0;
    
    if (var->step == 0) {
        // mirror position to target (half the distance):
    
        x = var_shared->target->pos_x;
        y = var_shared->target->pos_y;
        float x_obj = obj->pos_x;
        float y_obj = obj->pos_y;
    
        x = x_obj + (x_obj - x) / 2.0;
        y = y_obj + (y_obj - y) / 2.0;
        
        var->step++;
        
        return;
    }
    
    if (var->step == 1) {
        // go away from target:
        
        if (move_to_position(obj, x, y, vel_abs)) {
            var->step++;    // position reached
        }
        
        return;
    }
    
    if (var->step == 2) {
        // move to target:
        
        float x = var_shared->target->pos_x;
        float y = var_shared->target->pos_y;
        move_to_position(obj, x, y, vel_abs);
        
        if ((var->team_a && obj->pos_y > var->pos_y_default) ||
            (var->team_b && obj->pos_y < var->pos_y_default)) {
            
            // throw ball and stop:
            var_shared->target->disable_collision = false;
            throw(obj, 4.0);
            obj->vel_x = 0.0;
            obj->vel_y = 0.0;

            var->step = 0;
        }
        
        return;
    }
}

void task_flunky_player_retrieve_ball_or_target(
    object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
    if (var->step == 0) {
        // init:
        
        var_shared->target_retrieved = false;
        var_shared->ball_retrieved = false;
        var_shared->num_finished = 0;
        
        var->step = 1;
        
        return;
    }
    
    if (var->step == 1) {
        // get ball or bottle and pick it up:
        
        if (var_shared->target_retrieved && var_shared->ball_retrieved) {
            var->step = 3;
            return;
        }
        
        // hunt the object that is not yet retrieved. If both are not
        // retrieved, hunt the object that is closer:
        
        object_t* obj_hunted = var_shared->target;
        
        if (var_shared->target_retrieved && !var_shared->ball_retrieved) {
            
            obj_hunted = var_shared->ball;
            
        } else if 
            (!var_shared->target_retrieved && !var_shared->ball_retrieved) {
            
            if (squared_distance(obj, var_shared->target) >
                squared_distance(obj, var_shared->ball)) {
                
                obj_hunted = var_shared->ball;
            }
        }
        
        hunt_object(obj, &var->counter, var->clockwise, obj_hunted, dt);
        
        if (check_collision(obj, obj_hunted->id)) {
            
            pick_up(obj, obj_hunted);
            
            if (obj_hunted == var_shared->target) {
                var_shared->target_retrieved = true;
                var->step = 2;
            } else {
                var_shared->ball_retrieved = true;
                var->step = 3;
            }
        }
        
        return;
    }
    
    if (var->step == 2) {
        // place target:
        
        // reset hunt variables:
        obj->disable_damping = false;
        var->counter = 0;
        
        if(move_to_position(
            obj, var_shared->pos_x_target, var_shared->pos_y_target, 2.0)) {
            
            obj->vel_x = 0.0;
            obj->vel_y = 0.0;
            
            throw(obj, 0.0);
        
            var_shared->target->disable_collision = true;
        
            var->step = 3;
        }
        
        return;
    }
    
    
    if (var->step == 3) {
        // move to start position:
        
        // reset hunt variables:
        obj->disable_damping = false;
        var->counter = 0;
        
        if(move_to_position(obj, var->pos_x_default, var->pos_y_default, 2.0)) {
            obj->vel_x = 0.0;
            obj->vel_y = 0.0;
            if (var->team_a) {
                obj->facing = OBJECT_FACING_SOUTH;
            } else {
                obj->facing = OBJECT_FACING_NORTH;
            }
            
            var_shared->num_finished++;
            
            var->step = 666;
        }
        
        return;
    }
    
    if (var->step == 666) {
        
        if ((var->team_a && 
            var_shared->num_finished == var_shared->num_player_team_a)
            ||
            (var->team_b &&
            var_shared->num_finished == var_shared->num_player_team_b)) {
            
            var_shared->target_hit = false;
            
            if (var->team_a) {
                var_shared->team_a_turn = true;
                var_shared->team_b_turn = false;
            } else {
                var_shared->team_a_turn = false;
                var_shared->team_b_turn = true;
            }
        }
        
        return;
    }
}
