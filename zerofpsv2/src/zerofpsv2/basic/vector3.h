#ifndef _BASIC_VECTOR3_H_
#define _BASIC_VECTOR3_H_

#include <iostream>
#include <cmath>
#include "basic_x.h"

using namespace std;

class Matrix3;
class Vector4;

/**	\brief	A Class for vector math operations in 3D..
		\ingroup Basic
*/
class BASIC_API Vector3 
{
	public:
// Std Values
		static const Vector3 ZERO;									
		static const Vector3 AXIS_X;								
		static const Vector3 AXIS_Y;								
		static const Vector3 AXIS_Z;								
//		static const float degtorad;

// Data
		float x;
		float y;
		float z;                                
	
// Constructors
		Vector3() {	};											
		Vector3(float fX,float fY,float fZ) : x(fX), y(fY), z(fZ) { };

// Access 
		float& operator[] (const int i);

// Assignment 
		Vector3& operator=(const Vector3 &kOtherV3);
		void Set(float fXIn, float fYIn, float fZIn);
		void Zero();

// Comparison
		bool Compare(const Vector3& kOtherV3)				const;
		bool Compare( const Vector3& v, const float e ) const;		///< True if this is == v within the range of +-e
		bool operator==(const Vector3 &kOtherV3)	const;
		bool operator!=(const Vector3 &kOtherV3)	const;
		bool IsZero(void) const; 												///< True if this = <0,0,0>
		bool IsZero(const float e ) const; 									///< True if this is <0,0,0> within the range of +-e

// Arithmetic 
		Vector3 operator-() const;
		
		Vector3 operator+(const Vector3 &kOtherV3)	const;
		Vector3 operator-(const Vector3 &kOtherV3)	const;
		Vector3& operator+=(const Vector3 &kOtherV3); 
		Vector3& operator-=(const Vector3 &kOtherV3); 
		float operator*(const Vector3 &kOther)			const; 	//??

		Vector3 operator+(const float &fAdd)			const;
		Vector3 operator-(const float &fAdd)			const; 
		Vector3 operator*(const float &fOther)			const; 	
		Vector3 operator/(const float s )				const; 
		Vector3& operator*=(const float &fOther);
		Vector3& operator/=(const float fOther);
		Vector3& operator/=(const float& s );						//??

// Geometric
		Vector3 Cross( const Vector3& v )	const;						///< Return cross product between this vector and another one.
		float Dot( const Vector3& v  )		const;	 					///< Return dot product between this vec and another one.
		float Length(void) const;
		float LengthSqr(void) const;
		void Normalize(void);													///< Normalize this to unit vector.
		Vector3 Unit(void)						const;						///< Return unit vector of this.		

		// Vector operations.

// Other
		string ToString();

// NoSort
		Vector3 PEP(const Vector3 &fOther)	const;						//per element pruduct
		Vector3 Proj(const Vector3& v )		const;						///< Projects v on this.
		Vector3 Perp(const Vector3& v );										///< Perp v on this.
		float Angle(const Vector3& v );										///< Angle (in rad) between two vectors. 
		void Abs(void);															///< Do a fabs() on coo of this vectors.
		int	AbsMaxCoo(void);													///< Returns Index för den coo med > fabs(value).
		void Lerp(Vector3& from, Vector3& to, float flerp);			///< Linjär interpolering mellan två vectors.
		
		Vector3 Angels(void);
		Vector3 AToU();															///< convert Angel Vector to a Unit Vector

		// DistanceTo
      double DistanceTo (const Vector3& to) const;
		double DistanceTo (const Vector4& to) const;
      double DistanceXZTo (const Vector3& to) const;
};

#include "vector4.h"

BASIC_API Vector3 operator* ( const float& s, const Vector3& v );

BASIC_API Vector3 operator* (const Vector3& v, const Matrix3 m) ;
BASIC_API Vector3 operator* ( const Matrix3 m,const Vector3& v) ;

BASIC_API Vector3 operator/ (const Vector3& v, const Matrix3 m) ;
BASIC_API Vector3 operator/ ( const Matrix3 m,const Vector3& v) ;

// Assignment 
inline Vector3& Vector3::operator=(const Vector3 &kOtherV3) 
{
	x = kOtherV3.x;
	y = kOtherV3.y;
	z = kOtherV3.z;
	return *this;
}	

inline void Vector3::Set(float fXIn, float fYIn, float fZIn)
{
	x = fXIn;
	y = fYIn;
	z = fZIn;
}

inline void Vector3::Zero()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

// Comparison
inline bool Vector3::Compare(const Vector3& kOtherV3)		const
{
	return ((x == kOtherV3.x) && (y == kOtherV3.y) && (z == kOtherV3.z));
}

inline bool Vector3::Compare( const Vector3& v, const float e ) const	
{
	return (fabs(x-v.x) < e && fabs(y-v.y) < e && fabs(z-v.z) < e);
}

inline bool Vector3::operator==(const Vector3 &kOtherV3)	const 
{
	return Compare(kOtherV3);
}

inline bool Vector3::operator!=(const Vector3 &kOtherV3)	const 
{
	return !Compare(kOtherV3);
}

inline bool Vector3::IsZero(void) const 
{	
	return (x == 0.0f && y == 0.0f && z == 0.0f);
}

inline bool Vector3::IsZero( const float e ) const 				
{
	return (fabs(x) < e && fabs(y) < e && fabs(z) < e);
}

// Arithmetic operations
inline Vector3 Vector3::operator-() const 
{
		return Vector3(-x,-y,-z);
}

inline Vector3 Vector3::operator+(const Vector3 &kOtherV3)	const 
{
		return Vector3(x+kOtherV3.x,y+kOtherV3.y,z+kOtherV3.z);
}

inline Vector3 Vector3::operator-(const Vector3 &kOtherV3)	const 
{
		return Vector3(x-kOtherV3.x,y-kOtherV3.y,z-kOtherV3.z);
}

inline Vector3& Vector3::operator+=(const Vector3 &kOtherV3) 
{
		x += kOtherV3.x;
		y += kOtherV3.y;
		z += kOtherV3.z;
		return *this;
}

inline Vector3& Vector3::operator-=(const Vector3 &kOtherV3) 
{
		x -= kOtherV3.x;
		y -= kOtherV3.y;
		z -= kOtherV3.z;
		return *this;
}

inline float Vector3::operator*(const Vector3 &kOther)		const 
{
		return Dot(kOther);
}

inline Vector3 Vector3::operator+(const float &fAdd)		const 
{
		return Vector3(x+fAdd,y+fAdd,z+fAdd);
}

inline Vector3 Vector3::operator-(const float &fAdd)		const 
{
		return Vector3(x-fAdd,y-fAdd,z-fAdd);
}

inline Vector3 Vector3::operator*(const float &fOther)		const 
{
		return Vector3(x * fOther,y * fOther,z * fOther);
}

inline Vector3 Vector3::operator/( const float  s )	const 
{
		float invs = 1/s; 
		return Vector3( invs*x, invs*y, invs*z );
}

inline Vector3& Vector3::operator*=(const float &fOther) 
{
		x *= fOther;
		y *= fOther;
		z *= fOther;
		return *this;
}

inline Vector3& Vector3::operator/=(const float fOther) 
{
		x /= fOther;
		y /= fOther;
		z /= fOther;
		return *this;
}

inline Vector3& Vector3::operator /= ( const float& s ) 
{
		float r = 1 / s; x *= r; y *= r; z *= r; 
		return *this; 
}

// Geometric operatins
inline Vector3 Vector3::Cross( const Vector3& v )	const 
{					
	return Vector3( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x ); 
}

inline float Vector3::Dot( const Vector3& v  ) const
{		 					
		return x*v.x + y*v.y + z*v.z; 		
}	

inline float Vector3::Length(void) const 
{
		return (float) sqrt( x*x + y*y + z*z );  
}

inline float Vector3::LengthSqr(void) const 
{
		return (float)( x*x + y*y + z*z );  
}

inline void Vector3::Normalize(void)						
{
	float invlen = Length();
	
	if(invlen == 0.0)
		return;
	
	invlen = 1 / invlen;
	x *= invlen;
	y *= invlen;
	z *= invlen;
}

inline Vector3 Vector3::Unit(void) const						
{
	float invlen = Length();
	
	if(invlen == 0.0)
		return Vector3(0,0,0);
	
	invlen = 1 / invlen;
	return Vector3(x * invlen, y * invlen, z * invlen);
}
	



// DistanceTo
inline double Vector3::DistanceTo (const Vector3& to) const
{
	return sqrt( (x - to.x)*(x - to.x) + (y - to.y)*(y - to.y) + (z - to.z)*(z - to.z) );
}
	
inline double Vector3::DistanceTo (const Vector4& to) const 
{
	return sqrt( (x - to.x)*(x - to.x) + (y - to.y)*(y - to.y) + (z - to.z)*(z - to.z) );
}
	
inline double Vector3::DistanceXZTo (const Vector3& to) const 
{
	return sqrt( (x-to.x)*(x-to.x) + (z-to.z)*(z-to.z) );
}

// Vector operations.
inline void Vector3::Lerp(Vector3& from, Vector3& to, float flerp)
{
	*this = (to - from) * flerp + from;
}

#endif
















