#ifndef _BASIC_GLOBALS_H_
#define _BASIC_GLOBALS_H_

//#include <cstring>
#include <iostream>
#include "basic_x.h"
#include "os.h"
#include <cmath>

#include "vector3.h"

using namespace std;

// some god makros
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//some constants
#define PId2	1.5707963279489
#define PI		3.1415926535897932384626433832795f
#define zf_pi	6.28318531f
#define epsilon 1.0e-4f

float const degtorad = float(57.2957795130823208767981548141052f);




// angel funktions
float BASIC_API DegToRad(float fAngle);				///< Convert a angle in degrees to radians.
float BASIC_API RadToDeg(float fAngle);				///< Convert a angle in radians to degrees.
Vector3 BASIC_API GetYawVector2(float fAngleDeg);
float BASIC_API GetYawAngle(Vector3 kDir);


//character handling
void BASIC_API ConvertToLower(string& strText);
void BASIC_API Gemens(char* aText);									///< Turn a string into lowcase.
void BASIC_API IntToChar(char* aText,int iValue);				//
char* BoolStr(bool bFlag);												//returns string "true" or "false" i think	
bool BASIC_API IsSameIgnoreCase(const char* szString1, const char* szString2);

//error handling
extern char g_szFormatText[4096];	
BASIC_API void ZFError(const char *fmt, ...);
BASIC_API void ZFWarning(const char *fmt, ...);
void BASIC_API g_Logf(const char *fmt, ...);


// numerical funktions
int BASIC_API PowerOf2(int iVal);										//returns closest integer whit the power of 2
float BASIC_API Clamp(float fValue,float fBotom,float fTop);	//clamp value whitin botom and top
bool BASIC_API equal_f(float a, float b);
float BASIC_API round2(float val);										//round value to nearest integer


//strange funktions =D
Vector3 BASIC_API RndColor();												//randomize a colorvector...i think


#endif
