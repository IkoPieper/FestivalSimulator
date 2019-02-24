#ifndef _DATATYPES_H_
    #define _DATATYPES_H_

typedef struct listuint listuint_t;

struct listuint {
	listuint_t* next;
    unsigned int value;
};

void listuint_add(listuint_t* first, unsigned int value);
void listuint_remove(listuint_t* first, listuint_t* entry);

#endif
