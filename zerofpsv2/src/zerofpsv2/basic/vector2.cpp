#include "zfassert.h"
#include "vector2.h"


float Vector2::Dot( const Vector2& v  )
{
	return ( x*v.x + y*v.y );
}

void Vector2::Normalize(void)						
{
	float invlen = Length();
	ZFAssert(invlen != 0.0, "Vector2::Normalize: Vector has zero length");
	invlen = 1 / invlen;
	x *= invlen;
	y *= invlen;
}
