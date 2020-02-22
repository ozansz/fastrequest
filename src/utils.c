#include <stdio.h>
#include <stdint.h>

#include "fastrequest.h"

void FastRequest_Debug(const char *msg) {
#ifdef FR_DEBUG
    printf("\e[1;94m[DEBUG]\e[0m %s\n", msg);
#endif
}

void FastRequest_FuncDebug(const char *func, const char *msg) {
#ifdef FR_DEBUG
    printf("[[%s]] %s\n", func, msg);
#endif
}