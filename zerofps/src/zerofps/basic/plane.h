#ifndef _BASIC_PLANE_H_
#define _BASIC_PLANE_H_

#include "vector3.h"
#include "basic_x.h"
#include "os.h"

class BASIC_API Plane 
{
public:
	Vector3		kNormal;							// Normal of plane ( A,B,C in plane equation).
	float		fD;									// Distance ( D in plane equation).

	void		Set(float fX, float fY, float fZ, float fInD) {
		kNormal.Set(fX,fY,fZ);
		fD = fInD;
		}
	
	bool PointInside(Vector3 kPoint);
	bool SphereInside(Vector3 kCenter, float fRadius);
};

#endif






