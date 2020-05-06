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
			
            if (fabsf(obj->vel_x) < 0.1 && fabsf(obj->vel_y) < 0.1) {
                // start a stop animation:
				//obj->anim->cycle = obj->anim->cycle_first;
				if (obj->anim->id == 1) {
					object_select_animation(obj, 5);
				} else if (obj->anim->id == 2) {
					object_select_animation(obj, 6);
				} else if (obj->anim->id == 3) {
					object_select_animation(obj, 7);
				} else if (obj->anim->id == 4) {
					object_select_animation(obj, 8);
				}
				//obj->anim->cycle = obj->anim->cycle_first;
			} else {
                if (fabsf(obj->vel_y) > fabsf(obj->vel_x)) {
                    if (obj->vel_y < 0) {   // north
                        object_select_animation(obj, 1);
                    } else {                // south
                        object_select_animation(obj, 2);
                    }
                } else {
                    if (obj->vel_x < 0) {   // west
                        object_select_animation(obj, 3);
                    } else {                // east
                        object_select_animation(obj, 4);
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
