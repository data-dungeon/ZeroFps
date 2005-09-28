#ifndef _BASIC_MATH_H_
#define _BASIC_MATH_H_

#include <iostream>
#include <cmath>
#include "basic_x.h"

#include "quaternion.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix3.h"
#include "matrix4.h"


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

	static const float PI;					// pi			180
	static const float TWO_PI;				// pi * 2	360
	static const float HALF_PI;			// pi / 2	90
	static const float EPSILON;
};
#define epsilon 1.0e-4f


#endif
