#ifndef _BASIC_OS_H_
#define _BASIC_OS_H_
**************************
// Includes the correct operation system file.

#if defined(WIN32) 
#include "os_win32.h"
#endif

#if !defined(WIN32) 
#include "os_linux.h"
#endif

#endif




