#ifndef _DATATYPES_H_
    #define _DATATYPES_H_

#include <stdint.h>
#include <stdlib.h>

typedef struct listuint listuint_t;

struct listuint {
	listuint_t* next;
    uint32_t value;
};

listuint_t* listuint_add(listuint_t* first, uint32_t value);
listuint_t* listuint_remove(listuint_t* first, listuint_t* entry);
listuint_t* listuint_free(listuint_t* first);

#endif
