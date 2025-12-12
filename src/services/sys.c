#ifndef __CKESTER_SERVICES_SYS_C__
#define __CKESTER_SERVICES_SYS_C__

#include <stdlib.h>
#include <stdio.h>
#include "../components/string.c"

int ckester_sys_execute(Ckester_String* command) {
    if (!command || !command->data) {
        return -1;
    }
    return system(command->data);
}

#endif
