#ifndef _BASIC_GLOBALS_H_
#define _BASIC_GLOBALS_H_

//#include <cstring>
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

/*#define DEBUG_PRINT 0
#define printf(x) { int apa = 2; }*/

float const degtorad = float(57.2957795130823208767981548141052f);

float BASIC_API DegToRad(float fAngle);				///< Convert a angle in degrees to radians.
float BASIC_API RadToDeg(float fAngle);				///< Convert a angle in radians to degrees.

void BASIC_API Gemens(char* aText);					///< Turn a string into lowcase.
void BASIC_API IntToChar(char* aText,int iValue);

int BASIC_API PowerOf2(int iVal);

float BASIC_API Clamp(float fValue,float fBotom,float fTop);

Vector3 BASIC_API GetYawVector2(float fAngleDeg);

float BASIC_API GetYawAngle(Vector3 kDir);

char* BoolStr(bool bFlag);
void BASIC_API g_Logf(const char *fmt, ...);
Vector3 BASIC_API RndColor();

bool BASIC_API IsSameIgnoreCase(const char* szString1, const char* szString2);

#define PId2	1.5707963279489
#define PI		3.1415926535897932384626433832795f
#define zf_pi	6.28318531f
#define epsilon 1.0e-4f
bool BASIC_API equal_f(float a, float b);

float BASIC_API round2(float val);

extern char g_szFormatText[4096];	

BASIC_API void ZFError(const char *fmt, ...);
BASIC_API void ZFWarning(const char *fmt, ...);

#endif
