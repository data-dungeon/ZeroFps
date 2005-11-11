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

namespace Math
{
	float BASIC_API 	DegToRad(float fAngle);
	float BASIC_API 	RadToDeg(float fAngle);
	int BASIC_API 		PowerOf2(int iVal);
	Vector3 BASIC_API	GetYawVector2(float fAngleDeg);
	float BASIC_API	GetYawAngle(Vector3 kDir);
	float BASIC_API	Round2(float val);
	float BASIC_API	Randomf(float fInterval);
	int BASIC_API		Randomi(int iInterval);

	float BASIC_API	Clamp(float fValue,float fBotom,float fTop);	//clamp value whitin botom and top
	bool BASIC_API		Equal_f(float a, float b);

	const float BASIC_API	PI 		= 3.1415926535897932384626433832795f;;					// pi			180
	const float BASIC_API	TWO_PI	= 2.0 * Math::PI;				// pi * 2	360
	const float BASIC_API	HALF_PI	= 0.5 * Math::PI;			// pi / 2	90
	const float BASIC_API	EPSILON	= 1.0e-4f;
};
//#define epsilon 1.0e-4f


#endif
