#include <cmath>
#include "zfassert.h"
#include "vector4.h"
#include "matrix4.h"
#include "vector3.h"

using namespace std;


Vector4::Vector4(const Vector3 &kOtherV3)
: x(kOtherV3.x), y(kOtherV3.y), z(kOtherV3.z), w(1.0) { }; 

Vector4& Vector4::operator=(const Vector3 &kOtherV3) 
{
	x=kOtherV3.x;
	y=kOtherV3.y;
	z=kOtherV3.z;
	w=1;	
	return *this;
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


float &Vector4::operator[](const int i)
{
	return *((&x) + i);
//	ZFAssert(0, "Vector4::operator[]: Index out of range");
}


Vector4 Vector4::operator*(const Matrix4 &f) const 
{
	Vector4 ny;
	ny.Set(0,0,0,0);
	
	
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
}



float Vector4::Dot( const Vector4& v  ) const	
{
	return x*v.x + y*v.y + z*v.z+ w*v.w; 
}

void Vector4::Normalize(void)						
{
	float invlen = Length();
	ZFAssert(invlen != 0.0, "Vector4::Normalize: Vector has zero length ");
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
	ZFAssert(invlen != 0.0, "Vector4::PlainNormalize: Vector has zero length ");
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

void Vector4::Lerp(Vector4& from, Vector4& to, float flerp)
{
	*this = (to - from) * flerp  + from;
	//*this *= flerp;
	//*this += from;
}
