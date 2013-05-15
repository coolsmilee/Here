#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG_MODE
#define dbg printf
#else
#define dbg /\
/printf
#endif

#endif
