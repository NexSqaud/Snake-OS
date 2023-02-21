#ifndef _STDDEF_H
#define _STDDEF_H

// check size in compile time, cause warning
#define COMPILE_WARN_SIZEOF(x) char (*__kaboom)[sizeof(x)] = 1;

#define NULL (void*)0

#define true 1
#define false 0

#define stringify(x) __internal_str(x)
#define __internal_str(x) #x

#endif