#include "datatypes.h"

listuint_t* listuint_add(listuint_t* first, uint32_t value) {
	
	listuint_t* entry = (listuint_t*) malloc(sizeof(listuint_t));
	entry->value = value;
	entry->next = first;
	
	return(entry);  // new first element
	
}

// This remove function is cost effective for short lists. 
// For long lists consider an additional previous pointer.
listuint_t* listuint_remove(listuint_t* first, listuint_t* entry) {
	
	listuint_t* tmp = first;
	
	if (tmp == entry) {
		tmp = entry->next;
		free(entry);
		return(tmp);  // new first element
	}
	
	while (tmp->next != entry) {
		tmp = tmp->next;
	}
	tmp->next = entry->next;
	free(entry);
	return(first);
	
}

listuint_t* listuint_free(listuint_t* first) {
    while (first != NULL) {
        listuint_t* tmp = first;
        first = first->next;
        free(tmp);
    }
    return(NULL);
}
