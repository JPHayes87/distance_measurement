#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>
#include <String.h>
#include <stdint.h>

#define DEBUG_TAG "DEBUG: "

#define DEBUG

/*This function is basically printf, but if DEBUG is not defined the messages won't print.*/
void debug(const char* data, ...);

#endif