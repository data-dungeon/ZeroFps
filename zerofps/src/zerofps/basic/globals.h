#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <cstring>
#include <iostream>
#include "basic_x.h"
#include "os.h"

using namespace std;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//#define min(a,b) ((a) < (b) ? (a)  : (b))

float const degtorad = float(57.3248);

float BASIC_API DegToRad(float fAngle);
float BASIC_API RadToDeg(float fAngle);

void BASIC_API Gemens(char* aText);
void BASIC_API IntToChar(char* aText,int iValue);

#endif
