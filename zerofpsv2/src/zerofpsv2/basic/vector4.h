#ifndef _BASIC_VECTOR4_H_
#define _BASIC_VECTOR4_H_

#include <iostream>
#include "basic_x.h"
#include "zfassert.h"

class Vector3;
class Matrix4;

using namespace std;

/**	\brief	A Class for vector math operations in 4D
		\ingroup Basic
*/
class BASIC_API Vector4 {
	public:

// Data
		float x;
		float y;
		float z;
		float w;
	
// Constructors
		Vector4() { }
		explicit Vector4(float fX,float fY,float fZ,float fW) : x(fX), y(fY), z(fZ), w(fW) { }; 
		//Vector4(const Vector4 &kOtherV4) : x(kOtherV4.x), y(kOtherV4.y), z(kOtherV4.z), w(kOtherV4.w) { }; 
		//explicit Vector4(const Vector3 &kOtherV3);
	
// Access 
		float &operator[](const int i);

// Assignment 
		Vector4& operator=(const Vector4 &kOtherV4);
		Vector4& operator=(const Vector3 &kOtherV3);
		Vector4& operator=(const int &iEquals); 
		void Set(float fX, float fY, float fZ,float fW);
		void Zero();

// Comparison
		bool Compare(const Vector4& kOtherV4)				const;
		bool Compare( const Vector4& v, const float e ) const;		///< True if this is == v within the range of +-e
		bool operator==(const Vector4 &kOtherV3)	const;
		bool operator!=(const Vector4 &kOtherV3)	const;
		bool IsZero(void) const; 												///< True if this = <0,0,0>
		bool IsZero(const float e ) const; 									///< True if this is <0,0,0> within the range of +-e

// Arithmetic
		Vector4 operator-() ;

		Vector4 operator+(const Vector4 &kOtherV4)	const; 
		Vector4 operator-(const Vector4 &kOtherV4)	const; 
		Vector4& operator+=(const Vector4 &kOtherV4);
		Vector4& operator-=(const Vector4 &kOtherV4);
	
		
		Vector4 operator+(const float &fAdd)			const;
		Vector4 operator-(const float &fAdd)			const;
		Vector4 operator*(const float &fOther)			const; 
		Vector4 operator/(const float &fOther)			const; 
		Vector4& operator*=(const float &fOther);
		Vector4& operator/=(const float &fOther);
	
// Geometric 
		float Dot( const Vector4& v  )		const; 	
		float Length(void)const;		
		float LengthSqr(void) const;
		void Normalize(void);		

// Vector Operations.
		void Lerp(Vector4& from, Vector4& to, float flerp);			///< Linjär interpolering mellan två vectors.

// Other
		string ToString();

// NoSort
		Vector4 operator*(const Matrix4 &f)	const;
		float PlainLength(void)const;				
		void PlainNormalize(void);			
};

#include "vector3.h"

// Constructors

// Access 
inline float &Vector4::operator[](const int i)
{
	return *((&x) + i);
//	ZFAssert(0, "Vector4::operator[]: Index out of range");
}

// Assignment 
inline Vector4& Vector4::operator=(const Vector4 &kOtherV4) 
{
	x=kOtherV4.x;
	y=kOtherV4.y;
	z=kOtherV4.z;
	w=kOtherV4.w;	
	return *this;
}

inline Vector4& Vector4::operator=(const Vector3 &kOtherV3) 
{
	x=kOtherV3.x;
	y=kOtherV3.y;
	z=kOtherV3.z;
	w=1;	
	return *this;
}

inline Vector4& Vector4::operator=(const int &iEquals)
{
	x=float(iEquals);
	y=float(iEquals);
	z=float(iEquals);
	w=float(iEquals);
	return *this;
}

inline void Vector4::Set(float fX, float fY, float fZ,float fW) 
{
	x = fX;
	y = fY;
	z = fZ;
	w = fW;	
}

inline void Vector4::Zero()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
}

// Comparison
inline bool Vector4::Compare(const Vector4& kOtherV4)		const
{
	return (x == kOtherV4.x) && (y && kOtherV4.y) && (z && kOtherV4.z) && (w && kOtherV4.w);
}

inline bool Vector4::Compare( const Vector4& v, const float e ) const	
{
	return (fabs(x-v.x) < e && fabs(y-v.y) < e && fabs(z-v.z) < e && fabs(w-v.w) < e );
}

inline bool Vector4::operator==(const Vector4 &kOtherV3) const
{
	if(x == kOtherV3.x &&
		y == kOtherV3.y &&
		z == kOtherV3.z &&
		w == kOtherV3.w)

		return true;

	return false;
}

inline bool Vector4::operator!=(const Vector4 &kOtherV3) const
{
	return !(kOtherV3 == *this); 
}

inline bool Vector4::IsZero(void) const 
{	
	return (x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f);
}

inline bool Vector4::IsZero( const float e ) const 				
{
	return (fabs(x) < e && fabs(y) < e && fabs(z) < e && fabs(w) < e );
}

// Assignment 




// Arithmetic
inline Vector4 Vector4::operator-() 
{
	return Vector4(-x,-y,-z,-w);
}

inline Vector4 Vector4::operator+(const Vector4 &kOtherV4)	const 
{
	return Vector4(x+kOtherV4.x,y+kOtherV4.y,z+kOtherV4.z,w+kOtherV4.w);
}

inline Vector4 Vector4::operator-(const Vector4 &kOtherV4)	const 
{
	return Vector4(x-kOtherV4.x,y-kOtherV4.y,z-kOtherV4.z,w-kOtherV4.w);
}

inline Vector4& Vector4::operator+=(const Vector4 &kOtherV4) 
{
	x += kOtherV4.x;
	y += kOtherV4.y;
	z += kOtherV4.z;
	w += kOtherV4.w;	
	return *this;
}

inline Vector4& Vector4::operator-=(const Vector4 &kOtherV4) 
{
	x -= kOtherV4.x;
	y -= kOtherV4.y;
	z -= kOtherV4.z;
	w -= kOtherV4.w;
	return *this;
}

inline Vector4 Vector4::operator+(const float &fAdd)	const 
{
	return Vector4(x+fAdd,y+fAdd,z+fAdd,w+fAdd);
}

inline Vector4 Vector4::operator-(const float &fAdd)	const 
{
	return Vector4(x-fAdd,y-fAdd,z-fAdd,w-fAdd);
}

inline Vector4 Vector4::operator*(const float &fOther)		const 
{
	return Vector4(x * fOther,y * fOther,z * fOther,w * fOther);
}

inline Vector4 Vector4::operator/( const float &fOther )	const 
{
		float invs = 1/fOther; 
		return Vector4( invs*x, invs*y, invs*z, invs*w );
}

inline Vector4& Vector4::operator*=(const float &fOther) 
{
	x *= fOther;
	y *= fOther;
	z *= fOther;
	w *= fOther;
	return *this;
}

inline Vector4& Vector4::operator/=(const float &fOther) 
{
	x /= fOther;
	y /= fOther;
	z /= fOther;
	w /= fOther;
	return *this;
}

// Geometric 
inline float Vector4::Dot( const Vector4& v  ) const	
{
	return x*v.x + y*v.y + z*v.z+ w*v.w; 
}

inline float Vector4::Length(void) const
{
	return (float)sqrt( x*x + y*y + z*z +w*w );  
}

inline float Vector4::LengthSqr(void) const 
{
	return (float)( x*x + y*y + z*z + w*w );  
}

inline void Vector4::Normalize(void)						
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

// Vector Operations.
inline void Vector4::Lerp(Vector4& from, Vector4& to, float flerp)
{
	*this = (to - from) * flerp  + from;
}

#endif
















