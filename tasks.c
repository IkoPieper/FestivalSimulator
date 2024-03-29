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

void* tasks_get_task_variables(object_t* obj, uint32_t id) {
    
    task_t* tsk = tasks_get_task(obj, id);
    return(tsk->variables);
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

team_game_t* tasks_init_team_game() {
    
    team_game_t* t = (team_game_t*) malloc(sizeof(team_game_t));
    
    t->team_a = false;
    t->team_b = false;
    
    return(t);
}

void tasks_free_team_game(team_game_t* t) {
    
    free(t);
}

team_game_shared_t* tasks_init_team_game_shared(
    uint8_t num_player_team_a, uint8_t num_player_team_b) {
    
    team_game_shared_t* t = 
        (team_game_shared_t*) malloc(sizeof(team_game_shared_t));
    
    t->num_finished_a = 0;
    t->num_finished_b = 0;
    t->team_a_turn = true;
    t->team_b_turn = false;
    
    t->num_player_team_a = 0;
    t->num_player_team_b = 0;
    // will be increased if players are added to team pointer arrays
    
    // malloc team pointer arrays:
    t->team_a = (object_t**) malloc(num_player_team_a * sizeof(object_t*));
    t->team_b = (object_t**) malloc(num_player_team_b * sizeof(object_t*));
    
    return(t);
}

void tasks_free_team_game_shared(team_game_shared_t* t) {
    
    free(t->team_a);
    free(t->team_b);
    free(t);
}

void tasks_team_a_add(
    team_game_t* t, team_game_shared_t* t_shared, object_t* obj) {
    
    t->team_a = true;
    t->team_b = false;
    if (obj != NULL) {
        t_shared->team_a[t_shared->num_player_team_a++] = obj;
    }
}

void tasks_team_b_add(
    team_game_t* t, team_game_shared_t* t_shared, object_t* obj) {
    
    t->team_a = false;
    t->team_b = true;
    if (obj != NULL) {
        t_shared->team_b[t_shared->num_player_team_b++] = obj;
    }
}

bool tasks_is_players_turn(team_game_t* t, team_game_shared_t* t_shared) {

    return(
        (t->team_a && t_shared->team_a_turn) ||
        (t->team_b && t_shared->team_b_turn));
}

void tasks_team_starts(team_game_t* t, team_game_shared_t* t_shared) {
    
    if (t->team_a) {
        t_shared->num_finished_a = 0;
    } else {
        t_shared->num_finished_b = 0;
    }
}

void tasks_team_player_finished(team_game_t* t, team_game_shared_t* t_shared) {
    
    if (t->team_a) {
        t_shared->num_finished_a++;
    } else {
        t_shared->num_finished_b++;
    }
}

bool tasks_team_has_finished(team_game_t* t, team_game_shared_t* t_shared) {
    
    return(
        (t->team_a && t_shared->num_finished_a == t_shared->num_player_team_a) ||
        (t->team_b && t_shared->num_finished_b == t_shared->num_player_team_b));
}

bool tasks_both_teams_have_finished(team_game_shared_t* t_shared) {
    
    return(
        t_shared->num_finished_a == t_shared->num_player_team_a &&
        t_shared->num_finished_b == t_shared->num_player_team_b);
}

void tasks_switch_teams(team_game_shared_t* t_shared) {
    
    if (t_shared->team_a_turn) {
        t_shared->team_a_turn = false;
        t_shared->team_b_turn = true;
    } else {
        t_shared->team_a_turn = true;
        t_shared->team_b_turn = false;
    }
}

void tasks_team_a_set_step(uint32_t step, team_game_shared_t* t, uint32_t id) {
    
    tasks_team_set_step(step, t->team_a, t->num_player_team_a, id);
}

void tasks_team_b_set_step(uint32_t step, team_game_shared_t* t, uint32_t id) {
    
    tasks_team_set_step(step, t->team_b, t->num_player_team_b, id);
}

void tasks_team_set_step(
    uint32_t step, object_t** team, uint8_t num_player, uint32_t id) {
    
    for (uint8_t n = 0; n < num_player; n++) {
        task_t* tsk = tasks_get_task(team[n], id);
        tsk->step = step;
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
        
        object_t* bus = object_get(obj, TASK_OBJECT_ID_BUS);
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
    
    switch (id) {
        case TASK_FLUNKY_0:
            var_shared->target = object_get((object_t*) lst_obj->entry, 898);
            var_shared->ball = object_get((object_t*) lst_obj->entry, 899);
            var_shared->pos_y_line_team_a = 1070;
            var_shared->pos_y_line_team_b = 1240;
            break;
        case TASK_FLUNKY_1:
            // change values if adding TASK_FLUNKY_1 game:
            var_shared->target = object_get((object_t*) lst_obj->entry, 898);
            var_shared->ball = object_get((object_t*) lst_obj->entry, 899);
            var_shared->pos_y_line_team_a = 1070;
            var_shared->pos_y_line_team_b = 1240;
            break;
    }
    
    // put the players (obj) into team arrays. also init the player task 
    // variables here as well. this is usually done later, in the first step 
    // inside the players task function, but here is nicer:
    
    var_shared->round_start = true;
    var_shared->round_end = false;
    var_shared->team_a_won = false;
    var_shared->target_hit = false;
    var_shared->ball_retrieved = true;
    var_shared->target_retrieved = true;
    
    var_shared->teams = tasks_init_team_game_shared(4, 4);
    
    object_t* obj;
    task_t* tsk;
    flunky_t* var;
    lst_obj = get_first(lst_obj);
    
    while (lst_obj != NULL) {
        
        // get object:
        obj = (object_t*) lst_obj->entry;
        
        tsk = tasks_get_task(obj, id);
        
        // malloc task variables of object:
        tsk->variables = (void*) malloc(sizeof(flunky_t));
        var = (flunky_t*) tsk->variables;
        
        var->teams = tasks_init_team_game();
        
        var->pos_x_default = obj->pos_x;
        var->pos_y_default = obj->pos_y;
        var->pos_x_next = 0.0;
        var->pos_y_next = 0.0;
        var->counter = 0;
        var->clockwise = false;
        
        // init task variables of object:
        switch (obj->id) {
            case TASK_OBJECT_ID_FLUNKY_0_TEAM_A_PLAYERS:
                var->is_player = true;
                var->is_beer = false;
                var->is_ball = false;
                var->is_target = false;
                var->beer = 
                    object_get((object_t*) lst_obj->entry, obj->id + 10);
                obj->facing = OBJECT_FACING_SOUTH;
                printf("object %d joined team a!\n", obj->id);
                tasks_team_a_add(var->teams, var_shared->teams, obj);
                break;
            
            case TASK_OBJECT_ID_FLUNKY_0_TEAM_B_PLAYERS:
                var->is_player = true;
                var->is_beer = false;
                var->is_ball = false;
                var->is_target = false;
                var->beer = 
                    object_get((object_t*) lst_obj->entry, obj->id + 10);
                obj->facing = OBJECT_FACING_NORTH;
                printf("object %d joined team b!\n", obj->id);
                tasks_team_b_add(var->teams, var_shared->teams, obj);
                break;
                
            case TASK_OBJECT_ID_FLUNKY_0_TEAM_A_BEERS:
                var->is_player = false;
                var->is_beer = true;
                var->is_ball = false;
                var->is_target = false;
                var->beer_amount = 100.0;
                tasks_team_a_add(var->teams, var_shared->teams, NULL);
                object_select_animation(obj, 0);
                break;
                
            case TASK_OBJECT_ID_FLUNKY_0_TEAM_B_BEERS:
                var->is_player = false;
                var->is_beer = true;
                var->is_ball = false;
                var->is_target = false;
                var->beer_amount = 100.0;
                obj->disable_collision = true;
                obj->can_be_carried = false;
                tasks_team_b_add(var->teams, var_shared->teams, NULL);
                object_select_animation(obj, 0);
                break;
                
            case TASK_OBJECT_ID_FLUNKY_0_TARGET:
                var->is_player = false;
                var->is_beer = false;
                var->is_ball = false;
                var->is_target = true;
                var_shared->pos_x_target = obj->pos_x;
                var_shared->pos_y_target = obj->pos_y;
                obj->disable_collision = true;
                obj->can_be_carried = false;
                obj->anim_walk = false;
                object_select_animation(obj, ANIMATION_UPRIGHT);
                break;
                
            case TASK_OBJECT_ID_FLUNKY_0_BALL:
                var->is_player = false;
                var->is_beer = false;
                var->is_ball = true;
                var->is_target = false;
                break;
        }
        
        lst_obj = lst_obj->next;
    }
}

void task_flunky_free_shared(void* var_shared, uint32_t id) {
    
    flunky_shared_t* var = (flunky_shared_t*) var_shared;
    
    tasks_free_team_game_shared(var->teams);
    free(var);
}

void task_flunky(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    flunky_t* var = (flunky_t*) tsk->variables;
    flunky_shared_t* var_shared = (flunky_shared_t*) tsk->variables_shared;
    
    if (var_shared->round_end) {
        
        if (var->is_player) {
            
            task_flunky_player_round_end(tsk, obj, var, var_shared, frame, dt);
        }
        
    } else if (var_shared->round_start) {
        
        if (var->is_player) {
            
            task_flunky_player_round_start(tsk, obj, var, var_shared, frame, dt);
        }
        
    } else {
    
        if (var->is_player) {
            
            task_flunky_player(tsk, obj, var, var_shared, frame, dt);
            
        } else if (var->is_target) {
            
            task_flunky_target(tsk, obj, var, var_shared, frame, dt);
            
        }
    }
}

void task_flunky_free(
    task_t* tsk, object_t* obj, groups_t* grp, 
    bool* keys, uint64_t frame, float dt) {
    
    flunky_t* var = (flunky_t*) tsk->variables;
    
    tasks_free_team_game(var->teams);
    free(var);
}

void task_flunky_player(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
        
    if (tasks_is_players_turn(var->teams, var_shared->teams)) {
        
        if (var_shared->target_hit) {
            
            task_flunky_player_drink(tsk, obj, var, var_shared, frame, dt);
            
        } else {
            
            if (obj->obj_carries == var_shared->ball) {
                
                task_flunky_player_throw_ball(
                    tsk, obj, var, var_shared, frame, dt);
            }
        }
        
    } else {    // other teams turn
        
        if (obj->obj_carries == var->beer) {
            
            task_flunky_player_stop_drinking(
                tsk, obj, var, var_shared, frame, dt);
        }
            
        task_flunky_player_retrieve_ball_or_target(
            tsk, obj, var, var_shared, frame, dt);
    }
}

void task_flunky_target(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
    if (obj->col != NULL) {
        var_shared->target->anim_walk = true;
    }
    
    if (check_collision(obj, var_shared->ball->id)) {
        var_shared->target_hit = true;
    }
}

void task_flunky_player_drink(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
    var->beer->can_be_carried = true;
    pick_up(obj, var->beer);
    var->beer->can_be_carried = false;
    
    flunky_t* var_beer = 
        (flunky_t*) tasks_get_task_variables(var->beer, tsk->id);
    
    var_beer->beer_amount -= dt / 10.0;
    
    
    if (var_beer->beer_amount == 100.0) {
        object_select_animation(var->beer, 0);
    } else if (var_beer->beer_amount > 85.0) {
        object_select_animation(var->beer, 1);
    } else if (var_beer->beer_amount > 68.0) {
        object_select_animation(var->beer, 2);
    } else if (var_beer->beer_amount > 51.0) {
        object_select_animation(var->beer, 3);
    } else if (var_beer->beer_amount > 34.0) {
        object_select_animation(var->beer, 4);
    } else if (var_beer->beer_amount > 17.0) {
        object_select_animation(var->beer, 5);
    } else if (var_beer->beer_amount > 0.0) {
        object_select_animation(var->beer, 6);
    } else {    // beer is empty
        object_select_animation(var->beer, 7);
        
        say_new(obj, "Fertig!", 60);
        throw(obj, 2.0);
        
        // check if all beers of the team are empty:
        bool beer_empty = true;
        object_t** team = NULL;
        uint8_t num_player = 0;
        
        if (var->teams->team_a) {
            team = var_shared->teams->team_a;
            num_player = var_shared->teams->num_player_team_a;
        } else {
            team = var_shared->teams->team_b;
            num_player = var_shared->teams->num_player_team_b;
        }
        
        flunky_t* var_tmp = NULL;   // beer of another team player
        
        for (int n = 0; n < num_player; n++) {
            
            var_tmp = (flunky_t*) tasks_get_task_variables(team[n], tsk->id);
            var_tmp = 
                (flunky_t*) tasks_get_task_variables(var_tmp->beer, tsk->id);
            
            if (var_tmp->beer_amount > 0.0) {
                beer_empty = false;
            }
        }
        
        if (beer_empty == true) {   // all beers of the team are empty
            
            tasks_team_a_set_step(0, var_shared->teams, tsk->id);
            tasks_team_b_set_step(0, var_shared->teams, tsk->id);
            
            if (var->teams->team_a) {
                var_shared->team_a_won = true;
            } else {
                var_shared->team_a_won = false;
            }
            
            var_shared->round_end = true;
        }
        
    }
}

void task_flunky_player_stop_drinking(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
    if (var->teams->team_a) {
        obj->facing = OBJECT_FACING_SOUTH;
    } else {
        obj->facing = OBJECT_FACING_NORTH;
    }
            
    put_down(obj);
    
    flunky_t* var_beer = 
        (flunky_t*) tasks_get_task_variables(var->beer, tsk->id);
    var->beer->pos_x = var_beer->pos_x_default;
    var->beer->pos_y = var_beer->pos_y_default;
}

void task_flunky_player_throw_ball(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        // mirror position to target (half the distance):
    
        float x = var_shared->target->pos_x;
        float y = var_shared->target->pos_y;
        float x_obj = obj->pos_x;
        float y_obj = obj->pos_y;
    
        var->pos_x_next = x_obj + (x_obj - x) / 2.0;
        var->pos_y_next = y_obj + (y_obj - y) / 2.0;
        
        tsk->step = 1;
        
    } else if (tsk->step == 1) {
        // go away from target:
        
        const float vel_abs = 1.0;
        
        if (move_to_position(obj, var->pos_x_next, var->pos_y_next, vel_abs)) {
            tsk->step = 2;    // position reached
        }
        
    } else if (tsk->step == 2) {
        // move to target:
        
        const float vel_abs = 4.0;
        
        float x = var_shared->target->pos_x;
        float y = var_shared->target->pos_y;
        move_to_position(obj, x, y, vel_abs);
        
        if ((var->teams->team_a && obj->pos_y > var->pos_y_default) ||
            (var->teams->team_b && obj->pos_y < var->pos_y_default)) {
            
            // throw ball and stop:
            var_shared->target->disable_collision = false;
            throw(obj, 4.0);
            obj->vel_x = 0.0;
            obj->vel_y = 0.0;

            tsk->step = 0;
        }
    }
}

void task_flunky_player_retrieve_ball_or_target(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        // init:
        
        tasks_team_starts(var->teams, var_shared->teams);
        var->counter = 0;
        
        if (var_shared->target_hit) {
            
            // retrieve target and ball:
            var_shared->target_retrieved = false;
            var_shared->ball_retrieved = false;
            tsk->step = 1;
            
        } else if (var_shared->ball->obj_carried_by == NULL &&
                   fabsf(var_shared->ball->vel_x) < 0.1 && 
                   fabsf(var_shared->ball->vel_y) < 0.1) {
            
            // only retrieve the ball: 
            var_shared->target_retrieved = true;
            var_shared->ball_retrieved = false;
            tsk->step = 1;
            
            var_shared->target->disable_collision = true;
        }
        
    } else if (tsk->step == 1) {
        // get ball or bottle and pick it up:
        
        if (var_shared->target_retrieved && var_shared->ball_retrieved) {
            tsk->step = 3;
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
            
            var->counter = 0;   // reset hunt counter
            
            obj_hunted->can_be_carried = true;
            if (!pick_up(obj, obj_hunted)) {
                obj_hunted->can_be_carried = false;
                return;
            }
            obj_hunted->can_be_carried = false;
            
            if (obj_hunted == var_shared->target) {
                var_shared->target_retrieved = true;
                tsk->step = 2;
            } else {
                var_shared->ball_retrieved = true;
                tsk->step = 4;
            }
        }
        
    } else if (tsk->step == 2) {
        // place target:
        
        if(move_to_position(
            obj, var_shared->pos_x_target, var_shared->pos_y_target, 2.0)) {
            
            put_down(obj);
            
            var_shared->target->vel_x = 0.0;
            var_shared->target->vel_y = 0.0;
        
            var_shared->target->disable_collision = true;
            var_shared->target->can_be_carried = false;
            
            var_shared->target->anim_walk = false;
            object_select_animation(var_shared->target, ANIMATION_UPRIGHT);
            
            tsk->counter = 0;
        
            tsk->step = 3;
        }
        
    } else if (tsk->step == 3) {
        // place target, wait:
        
        if (tsk->counter == (uint32_t) (60.0 / dt)) {
            tsk->counter = 0;
            tsk->step = 4;
        } else {
            tsk->counter++;
        }
        
    } else if (tsk->step == 4) {
        // move to start position:
        
        if(move_to_position(obj, var->pos_x_default, var->pos_y_default, 2.0)) {
            
            if (var->teams->team_a) {
                obj->facing = OBJECT_FACING_SOUTH;
            } else {
                obj->facing = OBJECT_FACING_NORTH;
            }
            
            tasks_team_player_finished(var->teams, var_shared->teams);
            
            tsk->step = TASK_FINAL_STEP;
        }
        
    } else if (tsk->step == TASK_FINAL_STEP) {
        
        if (tasks_team_has_finished(var->teams, var_shared->teams)) {
            
            var_shared->target_hit = false;
            
            tasks_team_a_set_step(0, var_shared->teams, tsk->id);
            tasks_team_b_set_step(0, var_shared->teams, tsk->id);
            
            tasks_switch_teams(var_shared->teams);
        }
    }
}

void task_flunky_player_round_end(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
    if (tsk->step == 0) {
        
        if (obj->obj_carries != NULL) {
            throw(obj, 2.0);
        }
        
        tasks_team_starts(var->teams, var_shared->teams);
        
        if ((var->teams->team_a && var_shared->team_a_won) ||
            (var->teams->team_b && !var_shared->team_a_won)) {
            // players team won
            if (said(obj)) {
                say_new(obj, "Yippie!", 120);
                tsk->step = 1;
            }
            
        } else {
            
            tsk->step = 1;
        }
        
    } else if (tsk->step == 1) {            
        
        if (said(obj)) {
            
            var->counter = 0;
            var->beer->disable_collision = false;
        
            tsk->step = 2;
        }
        
    } else if (tsk->step == 2) {
        
        hunt_object(obj, &var->counter, var->clockwise, var->beer, dt);
        
        if (check_collision(obj, var->beer->id)) {
            
            var->beer->can_be_carried = true;
            pick_up(obj, var->beer);
            var->beer->can_be_carried = false;
            
            tsk->step = 3;
        }
        
    } else if (tsk->step == 3) {
        
        float x = 1400.0;
        float y = 0.0;
        
        if (var->teams->team_a) {
            y = 1000.0;
        } else {
            y = 1300.0;
        }
                
        if (move_to_position(obj, x, y, 1.0)) {
            
            flunky_t* var_beer = 
                (flunky_t*) tasks_get_task_variables(var->beer, tsk->id);
            var_beer->beer_amount = 100.0;
            object_select_animation(var->beer, 0);
            
            tasks_team_player_finished(var->teams, var_shared->teams);
            tsk->step = 4;
        }
        
    } else if (tsk->step == 4) {
        
        if (tasks_both_teams_have_finished(var_shared->teams)) {
            
            tsk->step = 5;
        }
    
    } else if (tsk->step == 5) {
        
        tasks_team_starts(var->teams, var_shared->teams);
        
        tsk->step = 6;
        
    } else if (tsk->step == 6) {
        
        if(move_to_position(obj, var->pos_x_default, var->pos_y_default, 2.0)) {
            
            if (var->teams->team_a) {
                obj->facing = OBJECT_FACING_SOUTH;
            } else {
                obj->facing = OBJECT_FACING_NORTH;
            }
            
            put_down(obj);
            var->beer->disable_collision = true;
            
            tasks_team_player_finished(var->teams, var_shared->teams);
            
            tsk->step = TASK_FINAL_STEP;
        }
        
    } else if (tsk->step == TASK_FINAL_STEP) {
        
        if (tasks_both_teams_have_finished(var_shared->teams)) {
            
            tasks_team_a_set_step(0, var_shared->teams, tsk->id);
            tasks_team_b_set_step(0, var_shared->teams, tsk->id);
            
            var_shared->round_end = false;
            var_shared->round_start = true;
        }
    }
}

void task_flunky_player_round_start(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {
    
    if (!tasks_is_players_turn(var->teams, var_shared->teams)) {
    
        if (tsk->step == 0) {
            
            var_shared->target->disable_collision = false;
            var_shared->target_hit = true;
        }
        
        task_flunky_player_retrieve_ball_or_target(
            tsk, obj, var, var_shared, frame, dt);
    
        if (var_shared->target_hit == false) {
            
            var_shared->round_start = false;
        }
    }
}

void task_flunky_beer_round_start(
    task_t* tsk, object_t* obj, flunky_t* var, flunky_shared_t* var_shared, 
    uint64_t frame, float dt) {

}
