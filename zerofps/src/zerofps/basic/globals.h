#ifndef _BASIC_GLOBALS_H_
#define _BASIC_GLOBALS_H_

#include <cstring>
#include <iostream>
#include "basic_x.h"
#include "os.h"
#include <cmath>

#include "vector3.h"

using namespace std;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

float const degtorad = float(57.3248);

float BASIC_API DegToRad(float fAngle);				///< Convert a angle in degrees to radians.
float BASIC_API RadToDeg(float fAngle);				///< Convert a angle in radians to degrees.

void BASIC_API Gemens(char* aText);					///< Turn a string into lowcase.
void BASIC_API IntToChar(char* aText,int iValue);

int BASIC_API PowerOf2(int iVal);


Vector3 BASIC_API GetYawVector2(float fAngleDeg);

float BASIC_API GetYawAngle(Vector3 kDir);

char* BoolStr(bool bFlag);

#endif
