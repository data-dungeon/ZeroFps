#include <cmath>
//#include "zfassert.h"
#include "vector4.h"
//#include "matrix4.h"
#include "vector3.h"

using namespace std;

/*
const Vector4 Vector4::ZERO(0,0,0,0);
const Vector4 Vector4::AXIS_X(1,0,0,0);
const Vector4 Vector4::AXIS_Y(0,1,0,0);
const Vector4 Vector4::AXIS_Z(0,0,1,0);
const Vector4 Vector4::AXIS_Z(0,0,0,1);
*/

Vector4::Vector4(float fX,float fY,float fZ,float fW)
{
	x=fX;
	y=fY;
	z=fZ;
	w=fW;
}

Vector4::Vector4(void) 
{
	x=0;
	y=0;
	z=0;
	w=0;	
}

Vector4::Vector4(const Vector3 &kOtherV3) 
{
	x=kOtherV3.x;
	y=kOtherV3.y;
	z=kOtherV3.z;
	w=1;	
}


// Assign
Vector4 Vector4::operator=(const Vector4 &kOtherV3) 
{
	x=kOtherV3.x;
	y=kOtherV3.y;
	z=kOtherV3.z;
	w=kOtherV3.w;	
	return *this;
}
Vector4 Vector4::operator=(const Vector3 &kOtherV3) 
{
	x=kOtherV3.x;
	y=kOtherV3.y;
	z=kOtherV3.z;
	w=1;	
	return *this;
}


void Vector4::set(float nx, float ny, float nz,float nw)
{
	x = nx;
	y = ny;
	z = nz;
	w = nw;	
}

// Comparison
bool Vector4::operator==(const Vector4 &kOtherV3) const
{
	if(x == kOtherV3.x &&
		y == kOtherV3.y &&
		z == kOtherV3.z &&
		w == kOtherV3.w)

		return true;

	return false;
}

bool Vector4::operator!=(const Vector4 &kOtherV3) const
{
	return !(kOtherV3 == *this); 
}

// Arithmetic operations
Vector4 Vector4::operator+(const Vector4 &kOtherV3) const
{
	return Vector4(x+kOtherV3.x,y+kOtherV3.y,z+kOtherV3.z,w+kOtherV3.w);
}

Vector4 Vector4::operator+(const float &fAdd) const
{
	return Vector4(x+fAdd,y+fAdd,z+fAdd,w+fAdd);
}

Vector4 Vector4::operator+=(const Vector4 &kOtherV3) 
{
	x += kOtherV3.x;
	y += kOtherV3.y;
	z += kOtherV3.z;
	w += kOtherV3.w;	
	return *this;
}

Vector4 Vector4::operator-(const Vector4 &kOtherV3)  const
{
	return Vector4(x-kOtherV3.x,y-kOtherV3.y,z-kOtherV3.z,w-kOtherV3.w);
}

Vector4 Vector4::operator-(const float &fAdd) const
{
	return Vector4(x-fAdd,y-fAdd,z-fAdd,w-fAdd);
}


Vector4 Vector4::operator-=(const Vector4 &kOtherV3) 
{
	x -= kOtherV3.x;
	y -= kOtherV3.y;
	z -= kOtherV3.z;
	w -= kOtherV3.w;
	return *this;
}

Vector4 Vector4::operator*(const float &fOther)  const
{
	return Vector4(x * fOther,y * fOther,z * fOther,w * fOther);
}

Vector4 Vector4::operator*=(const float &fOther)
{
	x *= fOther;
	y *= fOther;
	z *= fOther;
	w *= fOther;	
	return *this;
}

Vector4 Vector4::operator-()
{
	return Vector4(-x,-y,-z,-w);
}

float &Vector4::operator[](const int i)
{
	switch(i){
		case 0:
			return x;
		case 1:
			return y;		
		case 2:
			return z;
		case 3:
			return w;
		default:
		//	ZFAssert(0, "Vector4::operator[]: Index out of range");
			return x;
		
	}
}

/*Vector4 Vector4::operator*(const Matrix4 &f) const 
{
	Vector4 ny;
	ny.set(0,0,0,0);
	
	
	for(int j=0;j<4;j++) 
		for(int i=0;i<4;i++)
			switch(i) {
				case 0:
					ny[j]+=x*f.data[i*4+j];break;
				case 1:
					ny[j]+=y*f.data[i*4+j];break;
				case 2:
					ny[j]+=z*f.data[i*4+j];break;
				case 3:
					ny[j]+=w*f.data[i*4+j];break;
			}
	return ny;
}*/



float Vector4::Dot( const Vector4& v  ) const	
{
	return x*v.x + y*v.y + z*v.z+ w*v.w; 
}

void Vector4::Normalize(void)						
{
	float invlen = Length();
//	ZFAssert(invlen != 0.0, "Vector4::Normalize: Vector has zero length ");
		//assert(invlen != 0.0);
	invlen = 1 / invlen;
	x *= invlen;
	y *= invlen;
	z *= invlen;
	w *= invlen;
}

void Vector4::PlainNormalize(void)						
{
	float invlen = PlainLength();
//	ZFAssert(invlen != 0.0, "Vector4::PlainNormalize: Vector has zero length ");
	//	assert(invlen != 0.0);
	invlen = 1 / invlen;
	x *= invlen;
	y *= invlen;
	z *= invlen;
	w *= invlen;
}



// Vector operations.
float Vector4::Length(void) const
{
	return (float)sqrt( x*x + y*y + z*z +w*w );  

}

float Vector4::PlainLength(void) const
{
	return (float)sqrt( x*x + y*y + z*z );  

}

/*
Vector4 Vector4::unit(void) const						
{
	float invlen = length();
	assert(invlen != 0.0);
	invlen = 1 / invlen;
	return Vector4(x * invlen, y * invlen, z * invlen);
	return *this;
}


Vector4 Vector4::cross( const Vector4& v )	const
{
	return Vector4( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x ); 
}

*/

