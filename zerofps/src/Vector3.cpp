#include <cmath>
#include <cassert>
#include "Vector3.h"

using namespace std;

const Vector3 Vector3::ZERO(0,0,0);
const Vector3 Vector3::AXIS_X(1,0,0);
const Vector3 Vector3::AXIS_Y(0,1,0);
const Vector3 Vector3::AXIS_Z(0,0,1);


Vector3::Vector3(float fX,float fY,float fZ)
{
	x=fX;
	y=fY;
	z=fZ;
}

Vector3::Vector3(void) 
{
	x=0;
	y=0;
	z=0;
}

// Assign
Vector3 Vector3::operator=(const Vector3 &kOtherV3) 
{
	x=kOtherV3.x;
	y=kOtherV3.y;
	z=kOtherV3.z;
	return *this;
}

void Vector3::set(float nx, float ny, float nz)
{
	x = nx;
	y = ny;
	z = nz;
}

// Comparison
bool Vector3::operator==(const Vector3 &kOtherV3) const
{
	if(x == kOtherV3.x &&
		y == kOtherV3.y &&
		z == kOtherV3.z)

		return true;

	return false;
}

bool Vector3::operator!=(const Vector3 &kOtherV3) const
{
	return !(kOtherV3 == *this); 
}

// Arithmetic operations
Vector3 Vector3::operator+(const Vector3 &kOtherV3) const
{
	return Vector3(x+kOtherV3.x,y+kOtherV3.y,z+kOtherV3.z);
}

Vector3 Vector3::operator+(const float &fAdd) const
{
	return Vector3(x+fAdd,y+fAdd,z+fAdd);
}

Vector3 Vector3::operator+=(const Vector3 &kOtherV3) 
{
	x += kOtherV3.x;
	y += kOtherV3.y;
	z += kOtherV3.z;
	return *this;
}

Vector3 Vector3::operator-(const Vector3 &kOtherV3)  const
{
	return Vector3(x-kOtherV3.x,y-kOtherV3.y,z-kOtherV3.z);
}

Vector3 Vector3::operator-(const float &fAdd) const
{
	return Vector3(x-fAdd,y-fAdd,z-fAdd);
}


Vector3 Vector3::operator-=(const Vector3 &kOtherV3) 
{
	x -= kOtherV3.x;
	y -= kOtherV3.y;
	z -= kOtherV3.z;

	return *this;
}

Vector3 Vector3::operator*(const float &fOther)  const
{
	return Vector3(x * fOther,y * fOther,z * fOther);
}

Vector3 Vector3::operator*=(const float &fOther)
{
	x *= fOther;
	y *= fOther;
	z *= fOther;
	return *this;
}

Vector3 Vector3::operator-()
{
	return Vector3(-x,-y,-z);
}

// Vector operations.
float Vector3::length(void) const
{
	return (float)sqrt( x*x + y*y + z*z );  

}

float Vector3::dot( const Vector3& v  ) const	
{
	return x*v.x + y*v.y + z*v.z; 
}

Vector3 Vector3::unit(void) const						
{
	float invlen = length();
	assert(invlen != 0.0);
	invlen = 1 / invlen;
	return Vector3(x * invlen, y * invlen, z * invlen);
	return *this;
}

void Vector3::normalize(void)						
{
	float invlen = length();
	assert(invlen != 0.0);
	invlen = 1 / invlen;
	x *= invlen;
	y *= invlen;
	z *= invlen;
}

Vector3 Vector3::cross( const Vector3& v )	const
{
	return Vector3( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x ); 
}



