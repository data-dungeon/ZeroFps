#ifndef _BASIC_MATH_H_
#define _BASIC_MATH_H_

#include <iostream>
#include <cmath>
#include "basic_x.h"

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"
#include "matrix3.h"
#include "matrix4.h"


using namespace std;

namespace BASIC_API Math
{
	float DegToRad(float fAngle);
	float RadToDeg(float fAngle);
	int PowerOf2(int iVal);
	Vector3 GetYawVector2(float fAngleDeg);
	float GetYawAngle(Vector3 kDir);
	float round2(float val);
	float Randomf(float fInterval);
	int Randomi(int iInterval);

	float Clamp(float fValue,float fBotom,float fTop);	//clamp value whitin botom and top
	bool equal_f(float a, float b);

	const float PI 		= 3.1415926535897932384626433832795f;;					// pi			180
	const float TWO_PI	= 2.0 * Math::PI;				// pi * 2	360
	const float HALF_PI	= 0.5 * Math::PI;			// pi / 2	90
	const float EPSILON	= 1.0e-4f;
};
#define epsilon 1.0e-4f


#endif
