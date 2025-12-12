#ifndef __CKESTER_SERVICES_SYS_C__
#define __CKESTER_SERVICES_SYS_C__

#include <stdio.h>
#include <stdlib.h>

int ckester_sys_execute(char* command) {
    if (!command) {
        return -1;
    }

    return system(command);
}

#endif
