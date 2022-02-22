#ifndef _LIST_H_
    #define _LIST_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct list list_t;

struct list {
    list_t* prev;
    list_t* next;
    uint32_t id;
    void* entry;
};

list_t* create(void* entry, uint32_t id);

/* Only call these delete functions if entries have been freed! */
list_t* delete_single(list_t* current);
void delete_all(list_t* current);

list_t* get_next(list_t* current);
list_t* get_prev(list_t* current);
list_t* get_first(list_t* current);
list_t* get_last(list_t* current);

list_t* find(list_t* current, void* entry);
list_t* find_id(list_t* current, uint32_t id);

void swap(list_t* a, list_t* b);

void take_out(list_t* current);

void insert_before(list_t* current, list_t* added);
void insert_after(list_t* current, list_t* added);
void insert_first(list_t* current, list_t* added);
void insert_last(list_t* current, list_t* added);

list_t* create_before(list_t* current, void* entry, uint32_t id);
list_t* create_after(list_t* current, void* entry, uint32_t id);

uint32_t count(list_t* current);
bool share_entry(list_t* lst1, list_t* lst2);

#endif
