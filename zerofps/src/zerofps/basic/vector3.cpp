#include <cmath>
#include "zfassert.h"
#include "vector3.h"

using namespace std;

const Vector3 Vector3::ZERO(0,0,0);
const Vector3 Vector3::AXIS_X(1,0,0);
const Vector3 Vector3::AXIS_Y(0,1,0);
const Vector3 Vector3::AXIS_Z(0,0,1);
const float Vector3::degtorad = float(57.3248);


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

void Vector3::Set(float nx, float ny, float nz)
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
float Vector3::Length(void) const
{
	return (float)sqrt( x*x + y*y + z*z );  

}

float Vector3::Dot( const Vector3& v  ) const	
{
	return x*v.x + y*v.y + z*v.z; 
}

Vector3 Vector3::Unit(void) const						
{
	float invlen = Length();
	ZFAssert(invlen != 0.0, "Vector3::Unit: Vector has zero length");
		//assert(invlen != 0.0);
	invlen = 1 / invlen;
	return Vector3(x * invlen, y * invlen, z * invlen);
	return *this;
}

void Vector3::Normalize(void)						
{
	float invlen = Length();
	ZFAssert(invlen != 0.0, "Vector3::Normalize: Vector has zero length");
		//assert(invlen != 0.0);
	invlen = 1 / invlen;
	x *= invlen;
	y *= invlen;
	z *= invlen;
}

Vector3 Vector3::Cross( const Vector3& v )	const
{
	return Vector3( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x ); 
}


bool Vector3::IsZero(void) const
{
	return (x == 0.0 && y == 0.0 && z == 0.0); 
}

bool Vector3::NearlyEquals( const Vector3& v, const float e ) const	
{
	return (fabs(x-v.x) < e && fabs(y-v.y) < e && fabs(z-v.z) < e);
}

bool Vector3::NearlyZero( const float e ) const 				
{
	return (fabs(x) < e && fabs(y) < e && fabs(z) < e);
}

Vector3 Vector3::operator /  ( const float  s )	const					
{
	float invs = 1/s; 
	return Vector3( invs*x, invs*y, invs*z );
}

Vector3& Vector3::operator /= ( const float& s )					
{
	float r = 1 / s; x *= r; y *= r; z *= r; 
	return *this; 
}

float Vector3::SquaredLength(void) const
{
	return x*x + y*y +z*z;  
}

Vector3 Vector3::Proj( Vector3& v )				
{
	float dot = this->Dot(v);
	float len = Length();
	len *= len;

	//	vector_c p = (dot / len) * (*this) ;	??
	Vector3 p = (*this);
	float scale = (dot / len);
	p *= scale;
	return p;
}

Vector3 Vector3::Perp( Vector3& v )				
{
	Vector3 p = v - Proj(v);
	return p;
}

float Vector3::Angle( Vector3& v )				
{
	return acos( Dot(v) ); 
}

void Vector3::Abs(void)								
{
	x = fabs(x);	y = fabs(y);	z = fabs(z); 
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

void Vector3::Lerp(Vector3& from, Vector3& to, float flerp)
{
	*this = to - from;
	*this *= flerp;
	*this += from;
}

void Vector3::Print(void)
{
	cout << "<" << x << "," << y << "," << z << ">";
}

// FRIEND FUNCTIONS
Vector3 operator * ( const float& s, const Vector3& v )
{
	return v * s;
}

ostream& operator<<(ostream& os, const Vector3 &v)
{
	os <<  2345; 
	return os;
}


Vector3 Vector3::Angels(void){
	Vector3 temp=Vector3(0,0,0);





	temp.x=-atan(y/z) * degtorad;	
/*	if(z<0)
		temp.x+=180;
*/

//	cout<<"BLUB: "<<z<<endl;
	if(z<0.5 && z>0)
		z=0.5;
	if(z>-0.5 && z<0)
		z=0.5;




	temp.y=atan(x/z) * degtorad;
	
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

float& Vector3::operator[] (int i) 
{
	if(i==0)	return x;
	if(i==1)	return y;
	if(i==2)	return z;
	return x;
}



