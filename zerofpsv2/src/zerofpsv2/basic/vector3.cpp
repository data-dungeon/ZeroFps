#include "zfassert.h"
#include "math.h"

using namespace std;

const Vector3 Vector3::ZERO(0,0,0);
const Vector3 Vector3::AXIS_X(1,0,0);
const Vector3 Vector3::AXIS_Y(0,1,0);
const Vector3 Vector3::AXIS_Z(0,0,1);
//const float Vector3::degtorad = float(57.3248);

// used by the new distance formula
#define SWAP(A, B, T) {T = A; A = B; B = T;}

Vector3 Vector3::Proj(const Vector3& v ) const			
{
	static float dot,len,scale;
	static Vector3 p;


	dot = Dot(v);
	len = Length();
	len *= len;

	//	vector_c p = (dot / len) * (*this) ;	??
	p = (*this);
	scale = (dot / len);
	p *= scale;
	return p;
}

Vector3 Vector3::Perp(const Vector3& v )				
{
	static Vector3 p;

	p = v - Proj(v);
	return p;
}

float Vector3::Angle(const Vector3& v )				
{
	return float(acos( Dot(v) )); 
}

void Vector3::Abs(void)								
{
	x = float(fabs(x));
	y = float(fabs(y));
	z = float(fabs(z)); 
}

int	Vector3::AbsMaxCoo(void)	
{
	Vector3 test = *this;
	test.Abs();
	if(x >= y && x >= z)	return 0;
	if(y >= x && y >= z)	return 1;
	if(z >= x && z >= y)	return 2;
	return 0;
}



string Vector3::ToString()
{
	char szVec[128];
	sprintf(szVec, "%f,%f,%f", x,y,z);
	return string(szVec);	
}


// FRIEND FUNCTIONS
Vector3 operator * ( const float& s, const Vector3& v )
{
	return v * s;
}

ostream& operator<<(ostream& os, const Vector3 &v)
{
	os << v.x << "," << v.y << "," << v.z; 
	return os;
}
 

Vector3 Vector3::Angels(void)
{
	Vector3 temp=Vector3(0,0,0);

	temp.x = Math::RadToDeg(float(-atan(y/z)));	
/*	if(z<0)
		temp.x+=180;
*/

	if(z<0.5 && z>0)
		z=0.5;
	if(z>-0.5 && z<0)
		z=0.5;




	temp.y = Math::RadToDeg(float(atan(x/z)));
	
	if(z<0)
		temp.y+=180;
	
/*	
	if(z<0)
		temp.y+=180;
*/

/*	temp.y=atan(x/z) * degtorad;
	if(z<0)
		temp.y+=180;*/
//	temp.z=-atan(x/y) * degtorad;
	
/*	
	if(y<0)
		temp.z+=180;
	if(x<0)
		temp.z+=180;*/

	
	return temp;
}


float& Vector3::operator[] (const int i) 
{
	return *((&x) + i);
}


Vector3 Vector3::AToU()
{
	Vector3 New(0,0,0);
	
/*
	New.y += float(cos((x+90)/degtorad));
	New.x += float(cos((y)/degtorad)*cos((x)/degtorad) );
	New.z += float(sin((y)/degtorad)*cos((x)/degtorad) );
	New.Normalize();
*/
	return New;
}

Vector3 Vector3::PEP(const Vector3 &fOther)		const {
	return Vector3(x * fOther.x,y * fOther.y,z * fOther.z);
}


Vector3 operator* (const Vector3& v, const Matrix3 m) 
{
	return Vector3(v.x*m.m_afData[0] + v.y*m.m_afData[1] + v.z*m.m_afData[2],
						v.x*m.m_afData[3] + v.y*m.m_afData[4] + v.z*m.m_afData[5],
						v.x*m.m_afData[6] + v.y*m.m_afData[7] + v.z*m.m_afData[8]);
}	

Vector3 operator* ( const Matrix3 m,const Vector3& v) 
{
	return Vector3(m.m_afData[0]*v.x + m.m_afData[1]*v.y + m.m_afData[2]*v.z,
						m.m_afData[3]*v.x + m.m_afData[4]*v.y + m.m_afData[5]*v.z,
						m.m_afData[6]*v.x + m.m_afData[7]*v.y + m.m_afData[8]*v.z);
}	

Vector3 operator/ ( const Matrix3 m,const Vector3& v) 
{
	return Vector3(m.m_afData[0]/v.x + m.m_afData[1]/v.y + m.m_afData[2]/v.z,
						m.m_afData[3]/v.x + m.m_afData[4]/v.y + m.m_afData[5]/v.z,
						m.m_afData[6]/v.x + m.m_afData[7]/v.y + m.m_afData[8]/v.z);
}	

Vector3 operator/ ( const Vector3& v ,const Matrix3 m) 
{
	return Vector3(v.x/m.m_afData[0] + v.y/m.m_afData[1] + v.z/m.m_afData[2],
						v.x/m.m_afData[3] + v.y/m.m_afData[4] + v.z/m.m_afData[5],
						v.x/m.m_afData[6] + v.y/m.m_afData[7] + v.z/m.m_afData[8]);
}	
/*
Vector3 Vector3::operator*(const Matrix4 &f) const 
{
	Vector3 ny;
	ny.Set(0,0,0);
	
	
	for(int j=0;j<3;j++) 
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
*/
// double Vector3::DistanceTo (const Vector4& to) const
// {
//    //return sqrt( pow(x - to.x,2) + pow(y - to.y, 2) + pow(z - to.z,2) );
// 	return sqrt( (x - to.x)*(x - to.x) + (y - to.y)*(y - to.y) + (z - to.z)*(z - to.z) );
// }

// double Vector3::DistanceTo (const Vector3& to) const
// {
// /*	float fX = x - to.x;
// 	float fY = y - to.y;
// 	float fZ = z - to.z;
// 
// 	// this is called "Taylor series distance" and is supposed to be faster than sqrt
// 	int iSwapValue;
// 
// 	int iDistance;
// 	int iX = (int)fabs(fX) * 1024;
// 	int iY = (int)fabs(fY) * 1024;
// 	int iZ = (int)fabs(fZ) * 1024;
// 
// 	if(iY < iX)
// 		SWAP(iX, iY, iSwapValue);
// 	if(iZ < iY)
// 		SWAP(iY, iZ, iSwapValue);
// 	if(iY < iX)
// 		SWAP(iX, iY, iSwapValue);
// 
// 	iDistance = (iZ + 11 * (iY >> 5) + (iX >> 2));
// 
// 	return((double)(iDistance >> 10));
// */
//    //return sqrt( pow(x - to.x,2) + pow(y - to.y, 2) + pow(z - to.z,2) );
// 	return sqrt( (x - to.x)*(x - to.x) + (y - to.y)*(y - to.y) + (z - to.z)*(z - to.z) );	
// }

// double Vector3::DistanceXZTo (const Vector3& to)
// {
//    //return sqrt( pow(x - to.x,2) + pow(z - to.z,2) );
// 	return sqrt( (x-to.x)*(x-to.x) + (z-to.z)*(z-to.z) );
// }
