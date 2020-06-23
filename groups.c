#include "groups.h"

void groups_free(groups_t* grp) {
    
    delete_all(grp->lst_securities);
    delete_all(grp->lst_dixis);
    delete_all(grp->lst_have_anim);
    delete_all(grp->lst_have_ways);
    delete_all(grp->lst_have_tsk);
    
    free(grp);
}
