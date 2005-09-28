#ifndef _VECTOR2_H_
#define _VECTOR2_H_

#include <iostream>
#include <cmath>
#include "basic_x.h"
#include "zfassert.h"

using namespace std;

/**	\brief	A Class for vector math operations in 2D.
		\ingroup Basic
*/
class BASIC_API Vector2
{
	public:
// Std Values
		static const Vector2 ZERO;									
		static const Vector2 AXIS_X;								
		static const Vector2 AXIS_Y;								
	
// Data
		float x;
		float y;

// Constructors
		Vector2() { };
		Vector2(float fX, float fY);
		Vector2(const Vector2& kIn);

// Access 
		float& operator[] (const int i);

// Assignment 
		Vector2& operator=(const Vector2 &kOtherV2);
		void Set(float fX, float fY);
		void Zero();

// Comparison
		bool Compare(const Vector2& kOtherV2)				const;
		bool Compare( const Vector2& v, const float e ) const;		///< True if this is == v within the range of +-e
		bool operator==(const Vector2 &kOtherV2)	const;			
		bool operator!=(const Vector2 &kOtherV2)	const;		
		bool IsZero() const;
		bool IsZero(const float e ) const; 									///< True if this is <0,0,0> within the range of +-e

// Arithmetic 
		Vector2 operator-();
		
		Vector2 operator+(const Vector2& kOtherV3)	const;
		Vector2 operator-(const Vector2& kOtherV3)	const;
		Vector2& operator+=(const Vector2& kOtherV3);
		Vector2& operator-=(const Vector2& kOtherV3);

		Vector2 operator+(const float &fAdd)			const;
		Vector2 operator-(const float &fAdd)			const; 
		Vector2 operator*(const float& fOther)			const;
		Vector2 operator/(const float& fOther)			const;
		Vector2& operator*=(const float& fOther);
		Vector2& operator/=(const float& fOther);

// Geometric 
		float Dot( const Vector2& v  ); 			///< Return dot product between this vec and another one.
		float Length(void) const;
		float LengthSqr(void) const;
		void  Normalize(void);						///< Normalize this to unit vector.
		
// Other
		void Clamp(Vector2& kMin, Vector2& kMax);
		string ToString();
};

// Constructors
inline Vector2::Vector2(float fX, float fY)
{
	x = fX;
	y = fY;
};

inline  Vector2::Vector2(const Vector2 &kIn) 
{
  x = kIn.x;
  y = kIn.y;
}

// Assignment 
inline Vector2& Vector2::operator=(const Vector2 &kOtherV2) 
{
	x=kOtherV2.x;
	y=kOtherV2.y;
	return *this;
}	

inline void Vector2::Set(float fX, float fY) 
{
	x = fX;
	y = fY;
}

inline void Vector2::Zero()
{
	x = 0.0f;
	y = 0.0f;
}

// Comparison
inline bool Vector2::Compare(const Vector2& kOtherV2)		const
{
	return (x == kOtherV2.x) && (y && kOtherV2.y);
}

inline bool Vector2::Compare( const Vector2& v, const float e ) const	
{
	return (fabs(x-v.x) < e && fabs(y-v.y) < e);
}

inline bool Vector2::operator==(const Vector2 &kOtherV2) const
{
	return Compare(kOtherV2);
}

inline bool Vector2::operator!=(const Vector2 &kOtherV2) const
{
	return !Compare(kOtherV2);
}

inline bool Vector2::IsZero(void) const
{
	return (x == 0.0f && y == 0.0f); 
}

inline bool Vector2::IsZero( const float e ) const 				
{
	return (fabs(x) < e && fabs(y) < e);
}

// Arithmetic operations
inline Vector2 Vector2::operator-() 
{
	return Vector2(-x,-y);
}

inline Vector2 Vector2::operator+(const Vector2 &kOtherV3)	const 
{
	return Vector2(x+kOtherV3.x,y+kOtherV3.y);
}

inline Vector2 Vector2::operator-(const Vector2 &kOtherV3)	const 
{
	return Vector2(x-kOtherV3.x,y-kOtherV3.y);
}

inline Vector2& Vector2::operator+=(const Vector2 &kOtherV3) 
{
	x += kOtherV3.x;
	y += kOtherV3.y;
	return *this;
}

inline Vector2& Vector2::operator-=(const Vector2 &kOtherV3) 
{
	x -= kOtherV3.x;
	y -= kOtherV3.y;
	return *this;
}

inline Vector2 Vector2::operator+(const float &fAdd)		const 
{
		return Vector2(x+fAdd,y+fAdd);
}

inline Vector2 Vector2::operator-(const float &fAdd)		const 
{
		return Vector2(x-fAdd,y-fAdd);
}

inline Vector2 Vector2::operator*(const float &fOther)		const 
{
	return Vector2(x * fOther,y * fOther);
}

inline Vector2 Vector2::operator/(const float &fOther) const
{
	float fInv = 1.0f / fOther;
	return Vector2( x * fInv, y * fInv );
}

inline Vector2& Vector2::operator*=(const float &fOther) 
{
	x *= fOther;
	y *= fOther;
	return *this;
}

inline Vector2& Vector2::operator/=(const float &fOther)
{
	float fInv = 1.0f / fOther;
	x *= fInv;
	y *= fInv;
	return *this;
}

// Geometric operatins
inline float Vector2::Dot( const Vector2& v  )
{
	return ( x*v.x + y*v.y );
}

inline float Vector2::Length(void) const 
{
	return (float) sqrt( x*x + y*y );  
}

inline float Vector2::LengthSqr(void) const 
{
	return (float) ( x*x + y*y );  
}

inline void Vector2::Normalize(void)						
{
	float invlen = Length();
	ZFAssert(invlen != 0.0, "Vector2::Normalize: Vector has zero length");
	invlen = 1 / invlen;
	x *= invlen;
	y *= invlen;
}

// Other
inline void Vector2::Clamp(Vector2& kMin, Vector2& kMax)
{
	if(x < kMin.x)
		x = kMin.x;
	else if(x > kMax.x)
		x = kMax.x;

	if(y < kMin.y)
		y = kMin.y;
	else if(y > kMax.y)
		y = kMax.y;
}



#endif



