#include "zfassert.h"
#include "vector2.h"

const Vector2 Vector2::ZERO(0,0);
const Vector2 Vector2::AXIS_X(1,0);
const Vector2 Vector2::AXIS_Y(0,1);

Vector2::Vector2(const Vector2 &kIn) 
{
  x = kIn.x;
  y = kIn.y;
}

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

bool Vector2::operator==(const Vector2 &kOtherV3) const
{
	if(x == kOtherV3.x &&
		y == kOtherV3.y)

		return true;

	return false;
}

bool Vector2::operator!=(const Vector2 &kOtherV3) const
{
	return !(kOtherV3 == *this); 
}

bool Vector2::IsZero(void) const
{
	return (x == 0.0f && y == 0.0f); 
}

float& Vector2::operator[] (const int i) 
{
	return *((&x) + i);
}
