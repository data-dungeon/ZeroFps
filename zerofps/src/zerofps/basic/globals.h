#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "basic_x.h"
#include <cstring>
#include <iostream>

using namespace std;

#define min(a,b) ((a) < (b) ? (a)  : (b))

float const degtorad=57.3248;

void BASIC_API Gemens(char* aText);
void BASIC_API IntToChar(char* aText,int iValue);

#endif
