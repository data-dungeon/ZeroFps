#include <cmath>
#include "zfassert.h"
#include "math.h"

using namespace std;

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
float Vector4::PlainLength(void) const
{
	return (float)sqrt( x*x + y*y + z*z );  
}



string Vector4::ToString()
{
	char szVec[128];
	sprintf(szVec, "%f,%f,%f,%f", x,y,z,w);
	return string(szVec);	
}


