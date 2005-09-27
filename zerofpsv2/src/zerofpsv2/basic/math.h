#ifndef _BASIC_MATH_H_
#define _BASIC_MATH_H_

#include <iostream>
#include <cmath>
#include "vector3.h"
#include "basic_x.h"

using namespace std;

class BASIC_API Math
{
public:
	static float DegToRad(float fAngle);
	static float RadToDeg(float fAngle);
	static int PowerOf2(int iVal);
	static Vector3 GetYawVector2(float fAngleDeg);
	static float GetYawAngle(Vector3 kDir);
	static float round2(float val);
	static float Randomf(float fInterval);
	static int Randomi(int iInterval);

	static float Clamp(float fValue,float fBotom,float fTop);	//clamp value whitin botom and top
	static bool equal_f(float a, float b);

};


#endif
