#include "list.h"

list_t* create(void* entry, uint32_t id) {
    
    list_t* current = (list_t*) malloc(sizeof(list_t));
    
    current->prev = NULL;
    current->next = NULL;
    current->entry = entry;
    current->id = id;
    
    return(current);
}

/* Only call delete functions if entries have been freed! */
list_t* delete_single(list_t* current) {
    
    list_t* left = current->next;   // return following or last element
    if (left == NULL) {
        left = current->prev;
    }
    
    take_out(current);
    free(current);
    
    return(left);
}

void delete_all(list_t* current) {
    
    list_t* tmp;
    
    current = get_first(current);
    while (current != NULL) {
        
        tmp = current;
        current = current->next;
        free(tmp);
    }
    
}

list_t* get_next(list_t* current) {
    
    return(current->next);
}

list_t* get_prev(list_t* current) {
    
    return(current->prev);
}

list_t* get_first(list_t* current) {
    
    if (current == NULL) {
        return(current);
    }
    
    while (current->prev != NULL) {
        current = current->prev;
    }
    
    return(current);
}

list_t* get_last(list_t* current) {
    
    if (current == NULL) {
        return(current);
    }
    
    while (current->next != NULL) {
        current = current->next;
    }
    
    return(current);
}

list_t* find(list_t* current, void* entry) {
    
    current = get_first(current);
    while (current != NULL) {
        
        if (current->entry == entry) {
            return(current);
        }
        
        current = current->next;
    }
    
    return(current);
    /* returns NULL if nothing is found */
}

list_t* find_id(list_t* current, uint32_t id) {
    
    current = get_first(current);
    while (current != NULL) {
        
        if (current->id == id) {
            return(current);
        }
        
        current = current->next;
    }
    
    return(current);
    /* returns NULL if nothing is found */
}



void swap(list_t* a, list_t* b) {
    /* We do not need to swap the list elements by rederecting the 
     * pointers. Instead, we can simply swap the entries: */
    void* entry_tmp = a->entry;
    uint32_t id_tmp = a->id;
    a->entry = b->entry;
    a->id = b->id;
    b->entry = entry_tmp;
    b->id = id_tmp;
}

void take_out(list_t* current) {
    
    if (current->prev != NULL) {
        current->prev->next = current->next;
    }
    
    if (current->next != NULL) {
        current->next->prev = current->prev;
    }
    
    current->prev = NULL;
    current->next = NULL;
    
    /* current is now removed from the list and is in a state as if it
     * has just been initialized with create(); */
}

void insert_before(list_t* current, list_t* added) {
    
    if (current == NULL || added == NULL) {
        return;
    }
    
    if (current->prev != NULL) {
        current->prev->next = added;
    }
    
    added->prev = current->prev;
    added->next = current;
    
    current->prev = added;
}

void insert_after(list_t* current, list_t* added) {
    
    if (current == NULL || added == NULL) {
        return;
    }
    
    if (current->next != NULL) {
        current->next->prev = added;
    }
    
    added->next = current->next;
    added->prev = current;
    
    current->next = added;
}

void insert_first(list_t* current, list_t* added) {
    
    if (current == NULL || added == NULL) {
        return;
    }
    
    current = get_first(current);
    current->prev = added;
    added->next = current;
}

void insert_last(list_t* current, list_t* added) {
    
    if (current == NULL || added == NULL) {
        return;
    }
    
    current = get_last(current);
    current->next = added;
    added->prev = current;
}

list_t* create_before(list_t* current, void* entry, uint32_t id) {
    
    list_t* added = create(entry, id);
    insert_before(current, added);
    
    return(added);
}

list_t* create_after(list_t* current, void* entry, uint32_t id) {
    
    list_t* added = create(entry, id);
    insert_after(current, added);
    
    return(added);
}

uint32_t count(list_t* current) {
    
    uint32_t n = 0;
    
    current = get_first(current);
    
    while (current != NULL) {
        n++;
        current = current->next;
    }
    
    return (n);
}

bool share_entry(list_t* lst1, list_t* lst2) {
    
    lst1 = get_first(lst1);
    list_t* lst2_first = get_first(lst2);
    
    while (lst1 != NULL) {
        
        lst2 = lst2_first;
        
        while (lst2 != NULL) {
            
            if (lst1->entry == lst2->entry) {
                return(true);
            }
            
            lst2 = lst2->next;
        }
        
        lst1 = lst1->next;
    }
    
    return(false);
}

// TEST FUNCTION
/*
#include <stdio.h>

int main(int argc, char** argv) {
    
    int a = 1;
    int b = 2;
    int c = 3;
    
    list_t* numbers = create(&a, 0);
    numbers = create_after(numbers, &b, 0);
    numbers = create_after(numbers, &c, 0);
    
    printf("Numbers in List:\n");
    list_t* current = get_first(numbers);
    while (current != NULL) {
        printf("%d\n", * (int*) current->entry);
        current = get_next(current);
    }
    
    printf("Find b:\n");
    current = find(numbers, &b);
    printf("%d\n", * (int*) current->entry);
    
    printf("Delete b:\n");
    delete_single(current);
    
    printf("Numbers in List:\n");
    current = get_first(numbers);
    while (current != NULL) {
        printf("%d\n", * (int*) current->entry);
        current = get_next(current);
    }
    
    printf("Add b again:\n");
    current = get_last(numbers);
    create_before(current, &b, 0);
    
    printf("Numbers in List:\n");
    current = get_first(numbers);
    while (current != NULL) {
        printf("%d\n", * (int*) current->entry);
        current = get_next(current);
    }
    
    printf("Swap a and c:\n");
    swap(get_first(numbers), get_last(numbers));
    
    printf("Numbers in List:\n");
    current = get_first(numbers);
    while (current != NULL) {
        printf("%d\n", * (int*) current->entry);
        current = get_next(current);
    }
    
    printf("Find a (=1):\n");
    current = find(numbers, &a);
    printf("%d\n", * (int*) current->entry);
    
    printf("Delete all:\n");
    delete_all(numbers);
    
    // The following should make valgrind mad:
    printf("Numbers in List:\n");
    current = get_first(numbers);
    while (current != NULL) {
        printf("%d\n", * (int*) current->entry);
        current = get_next(current);
    }
    
    return(0);
}*/
