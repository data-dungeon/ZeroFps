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
#include <vector>

using namespace std;

class BASIC_API Math
{
public:
	static float  	DegToRad(float fAngle);
	static float  	RadToDeg(float fAngle);
	static int  	PowerOf2(int iVal);
	static Vector3 GetYawVector2(float fAngleDeg);
	static float 	GetYawAngle(Vector3 kDir);
	static float 	Round2(float val);
	static float 	Randomf(float fInterval);
	static int 		Randomi(int iInterval);
	static void 	SetRandomSeed(unsigned int iSeed);   //if seed == 0 time is used
	static float 	Clamp(float fValue,float fBotom,float fTop);	//clamp value whitin botom and top
	static bool 	Equal_f(float a, float b);

	static const float PI ;					// pi			180
	static const float TWO_PI;				// pi * 2	360
	static const float HALF_PI;			// pi / 2	90
	static const float EPSILON;
	
	static void		GenerateTangents(const Vector3* akVertises,const Vector3* akNormals,const Vector2* akTexCoord,vector<Vector3>& kTangents,vector<Vector3>& kBiTangents,int iVertises);
	static void		GenerateTangents(const Vector3* akVertises,const Vector3* akNormals,const Vector2* akTexCoord,const int* aiFaces,vector<Vector3>& kTangents,vector<Vector3>& kBiTangents,int iFaces);

// 	const float PI 		= 3.1415926535897932384626433832795f;;					// pi			180
// 	const float TWO_PI	= 2.0 * Math::PI;				// pi * 2	360
// 	const float HALF_PI	= 0.5 * Math::PI;			// pi / 2	90
// 	const float EPSILON	= 1.0e-4f;
};



#endif
