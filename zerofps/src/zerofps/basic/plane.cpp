#include "zfassert.h"
#include "plane.h"


bool Plane::PointInside(Vector3 kPoint)
{
	return ((kNormal.Dot(kPoint) + fD) <= 0);
}

bool Plane::SphereInside(Vector3 kCenter, float fRadius)
{
	float d = kNormal.Dot(kCenter) + fD;
	if( d <= -fRadius )
		return false;

	return true;
}




