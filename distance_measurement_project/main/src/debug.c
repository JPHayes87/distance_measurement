#include <stdarg.h>
#include "debug.h"

/*This function is basically printf, but it DEBUG is not defined, the messages won't print.*/
void debug(const char* data, ...)
{
    #ifdef DEBUG

    va_list args;

    va_start(args, data);

    vfprintf(stdout, data, args);

    va_end(args);

    #endif
}