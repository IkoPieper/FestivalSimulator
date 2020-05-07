#include "on_loop.h"
#include "collisions.h"
#include "movements.h"
#include "animation.h"
#include "waypoints.h"

void on_loop(object_t* obj, verletbox_t* vbox, bool* keys, uint64_t frame) {
	
	//Uint32 time;
	
	//time = SDL_GetTicks();
	collisions(obj, vbox);
	//printf("time for collisions: %d\n", SDL_GetTicks() - time);
	
	//time = SDL_GetTicks();
	movements(obj, keys);
	//printf("time for movements: %d\n", SDL_GetTicks() - time);
	
    on_loop_tasks(obj, keys, frame);
    
	//time = SDL_GetTicks();
	on_loop_animations(obj, keys, frame);
	
	on_loop_waypoints(obj, frame);
	//printf("time for animations and waypoints: %d\n", SDL_GetTicks() - time);

}

// call all the task functions of the objects:
void on_loop_tasks(object_t* obj, bool* keys, uint64_t frame) {
    
    obj = object_get_first(obj);
	while (obj != NULL) {
        
        if (obj->tsk != NULL) {
            
            list_t* lst = get_first(obj->tsk);
            while (lst != NULL) {
                
                task_t* tsk = (task_t*) lst->entry;
                tsk->task_function(tsk, obj, keys, frame);
                
                lst = lst->next;
            }
        }
        
        obj = obj->next_object;
	}
}

void on_loop_animations(object_t* obj, bool* keys, uint64_t frame) {
	
	obj = object_get_first(obj);
	
	while (obj != NULL) {
		
		// select animation:
		if (obj->anim_walk) {
			
            // calculate actual velocity:
            float vel_x = obj->pos_x - obj->pos_x_old;
            float vel_y = obj->pos_y - obj->pos_y_old;
            
            if (fabsf(vel_x) < 0.5 && fabsf(vel_y) < 0.5) {
                // start a stop animation:
				//obj->anim->cycle = obj->anim->cycle_first;
				if (obj->anim->id == ANIMATION_WALK_NORTH) {
					object_select_animation(obj, ANIMATION_REST_NORTH);
				} else if (obj->anim->id == ANIMATION_WALK_SOUTH) {
					object_select_animation(obj, ANIMATION_REST_SOUTH);
				} else if (obj->anim->id == ANIMATION_WALK_WEST) {
					object_select_animation(obj, ANIMATION_REST_WEST);
				} else if (obj->anim->id == ANIMATION_WALK_EAST) {
					object_select_animation(obj, ANIMATION_REST_EAST);
				}
				//obj->anim->cycle = obj->anim->cycle_first;
			} else {
                if (fabsf(vel_y) > fabsf(vel_x)) {
                    if (vel_y < 0) {
                        object_select_animation(obj, ANIMATION_WALK_NORTH);
                    } else {
                        object_select_animation(obj, ANIMATION_WALK_SOUTH);
                    }
                } else {
                    if (vel_x < 0) {
                        object_select_animation(obj, ANIMATION_WALK_WEST);
                    } else {
                        object_select_animation(obj, ANIMATION_WALK_EAST);
                    }
                }
                // set speed of animation depending on object velocity:
                float abs_vel = fabsf(
                    obj->vel_x * obj->vel_x + obj->vel_y * obj->vel_y);
                int32_t delay_frames = (int32_t) 20 - 20 * abs_vel;
                if (delay_frames < 5) {
                    delay_frames = 5;
                }
                animation_t* anim = (animation_t*) obj->anim->entry;
                anim->delay_frames = delay_frames;
                    
            }
            
		}
        
		// animate:
		if (obj->anim != NULL) {
			object_animate(obj, frame);
		}
		obj = obj->next_object;
	}
	
}

void on_loop_waypoints(object_t* obj, uint64_t frame) {
	
	obj = object_get_first(obj);
	
	while (obj != NULL) {
        	
		if (obj->ways != NULL) {
            
            waypoints_t* ways = (waypoints_t*) obj->ways->entry;
        
            if (ways->active && !obj->vel_lock) {

                object_get_next_waypoint(obj);
                
            }
		}
	
		obj = obj->next_object;
	}
	
}
