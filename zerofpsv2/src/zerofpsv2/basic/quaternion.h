#ifndef _BASIC_QUATERNION_H_
#define _BASIC_QUATERNION_H_

#include <iostream>
#include "basic_x.h"
#include "math.h"

using namespace std;

class Vector3;
class Matrix3;
class Matrix4;

/**	\brief	A class for quaternions (4d num that describes rotation)
		\ingroup Basic
*/
class BASIC_API Quaternion	
{
public:
// Std Values
	static Quaternion ZERO;
	static Quaternion IDENTITY;

// Data
	float	x;
	float y;
	float z;
	float w;

// Constructors
	Quaternion();
	Quaternion( float fW, float fX, float fY, float fZ);
	Quaternion(const Quaternion& rkQ);

// Access 
   float& operator[] (int i);
	Vector3 GetVector3();

// Assignment 
	void Set( float fW, float fX, float fY, float fZ);	
	Quaternion& operator= (const Quaternion& rkQ );

// Comparison
	bool Compare(const Quaternion& kQ)						const;
	bool Compare(const Quaternion& kQ, const float e ) const;		///< True if this is == v within the range of +-e
	int operator == ( const Quaternion& rkQ ) const; 
	int operator != ( const Quaternion& rkQ ) const;

// Arithmetic
   Quaternion operator- () const;

	Quaternion operator+ (const Quaternion& rkQ ) const;
	Quaternion operator- (const Quaternion& rkQ ) const;
	Quaternion operator+= (const Quaternion& rkQ );
	Quaternion operator-= (const Quaternion& rkQ );

	Quaternion operator* (const Quaternion& rkQ ) const;
	Quaternion operator*= (const Quaternion& rkQ );

	Quaternion operator* (float fScalar) const;
	Quaternion operator/=(float s);
   friend Quaternion operator* (float fScalar, const Quaternion& rkQ);

// Geometric
	Quaternion conjugate(void) const;
	float Dot (const Quaternion& q) const;
	float Length(void);
	void Normalize(void);
	float Norm(void) const;
	Quaternion Inverse (void) const;
	Quaternion UnitInverse (void) const;
	Vector3 RotateVector3(const Vector3& v) const;

// Conversions
   void FromRotationMatrix (const Matrix3& kRot);
   void ToRotationMatrix (Matrix3& kRot) const;
	void FromRotationMatrix (const Matrix4& kRot);
   void ToRotationMatrix (Matrix4& kRot) const;
	void FromAngleAxis (const float& rfAngle, const Vector3& rkAxis);
   void ToAngleAxis (float& rfAngle, Vector3& rkAxis) const;
   void FromAxes (const Vector3* akAxis);
   void ToAxes (Vector3* akAxis) const;
	void AngleQuaternion( const Vector3 angles );

// Other
	string ToString();

// NoSort
	void QuaternionSlerp( Quaternion* from, Quaternion* to, float t);
};

BASIC_API Quaternion operator*(const Quaternion& q,const Vector3& v);
BASIC_API Quaternion operator*(const Vector3& v,const Quaternion& q);

// Constructors
inline Quaternion::Quaternion()
{

}

inline Quaternion::Quaternion( float fW, float fX, float fY , float fZ)
{
	w = fW;
	x = fX;
	y = fY;
	z = fZ;
}

inline Quaternion::Quaternion(const Quaternion& rkQ)
{
    w = rkQ.w;
    x = rkQ.x;
    y = rkQ.y;
    z = rkQ.z;
}

// Access 
inline float& Quaternion::operator[] (int i) 
{
	if(i==0)	return x;
	if(i==1)	return y;
	if(i==2)	return z;
	if(i==3)	return w;
	return x;
}

inline Vector3 Quaternion::GetVector3()
{
	return Vector3(x,y,z);
}

// Assignment
inline void Quaternion::Set( float fW, float fX, float fY , float fZ)
{
	w = fW;
	x = fX;
	y = fY;
	z = fZ;
}

inline Quaternion& Quaternion::operator= (const Quaternion& rkQ )
{
	w = rkQ.w;
   x = rkQ.x;
	y = rkQ.y;
   z = rkQ.z;
	return *this;	
}

// Comparison
inline bool Quaternion::Compare(const Quaternion& kQ) const
{
	return (x == kQ.x) && (y && kQ.y) && (z && kQ.z) && (w && kQ.w);
}

inline bool Quaternion::Compare( const Quaternion& kQ, const float e ) const
{
	return (fabs(x-kQ.x) < e && fabs(y-kQ.y) < e && fabs(z-kQ.z) < e && fabs(w-kQ.w) < e );
}

inline int Quaternion::operator== ( const Quaternion& rkQ ) const
{
	return (rkQ.x==x && rkQ.y==y && rkQ.z==z && rkQ.w == w);
}

inline int Quaternion::operator!= ( const Quaternion& rkQ ) const
{
	return !(rkQ == *this);
}

// Arithmetic
inline Quaternion Quaternion::operator- () const
{
    return Quaternion(-w,-x,-y,-z);
}

inline Quaternion Quaternion::operator + (const Quaternion& rkQ ) const 
{
	return Quaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);
}

inline Quaternion Quaternion::operator - (const Quaternion& rkQ ) const 
{
	return Quaternion(w-rkQ.w,x-rkQ.x,y-rkQ.y,z-rkQ.z);
}

inline Quaternion Quaternion::operator += (const Quaternion& rkQ )
{
    w += rkQ.w;
    x += rkQ.x;
    y += rkQ.y;
    z += rkQ.z;
	return *this;
}

inline Quaternion Quaternion::operator -= (const Quaternion& rkQ )
{
    w -= rkQ.w;
    x -= rkQ.x;
    y -= rkQ.y;
    z -= rkQ.z;
	return *this;
}

inline Quaternion Quaternion::operator * (const Quaternion& rkQ) const
{
    return Quaternion
    (
		w*rkQ.w - x*rkQ.x - y*rkQ.y - z*rkQ.z,
		w*rkQ.x + x*rkQ.w + y*rkQ.z - z*rkQ.y,
		w*rkQ.y + y*rkQ.w + z*rkQ.x - x*rkQ.z,
		w*rkQ.z + z*rkQ.w + x*rkQ.y - y*rkQ.x
    );

/*	Quaternion qtmp;
	
    qtmp.w		  = w * q.w		   - vector.x * q.vector.x - vector.y * q.vector.y - vector.z * q.vector.z;
    qtmp.vector.x = w * q.vector.x + vector.x * q.w		   + vector.y * q.vector.z - vector.z * q.vector.y;
    qtmp.vector.y = w * q.vector.y + vector.y * q.w        + vector.z * q.vector.x - vector.x * q.vector.z;
    qtmp.vector.z = w * q.vector.z + vector.z * q.w        + vector.x * q.vector.y - vector.y * q.vector.x;
	return qtmp;*/
}

inline Quaternion Quaternion::operator *= (const Quaternion& rkQ) 
{
    Quaternion res
    (
		w*rkQ.w - x*rkQ.x - y*rkQ.y - z*rkQ.z,
		w*rkQ.x + x*rkQ.w + y*rkQ.z - z*rkQ.y,
		w*rkQ.y + y*rkQ.w + z*rkQ.x - x*rkQ.z,
		w*rkQ.z + z*rkQ.w + x*rkQ.y - y*rkQ.x
    );

	*this = res;
	return res;
/*	Quaternion qtmp;
	
    qtmp.w		  = w * q.w		   - vector.x * q.vector.x - vector.y * q.vector.y - vector.z * q.vector.z;
    qtmp.vector.x = w * q.vector.x + vector.x * q.w		   + vector.y * q.vector.z - vector.z * q.vector.y;
    qtmp.vector.y = w * q.vector.y + vector.y * q.w        + vector.z * q.vector.x - vector.x * q.vector.z;
    qtmp.vector.z = w * q.vector.z + vector.z * q.w        + vector.x * q.vector.y - vector.y * q.vector.x;
	*this = qtmp;
	return qtmp;*/
}


inline Quaternion Quaternion::operator* (float fScalar) const
{
    return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
}

inline Quaternion Quaternion::operator/=(float s)
{
	w /= s;
	x /= s;
	y /= s;
	z /= s;
	
	return *this;
}

inline Quaternion operator* (float fScalar, const Quaternion& rkQ)
{
    return Quaternion(fScalar*rkQ.w,fScalar*rkQ.x,fScalar*rkQ.y, fScalar*rkQ.z);
}

// Geometric
inline Quaternion Quaternion::conjugate(void) const
{
	return Quaternion(
		-x,
		-y,
		-z,
		w);
}

inline float Quaternion::Dot (const Quaternion& rkQ) const
{
	return w*rkQ.w + x*rkQ.x + y*rkQ.y + z*rkQ.z;
}

inline float Quaternion::Length(void)
{
	return sqrt(Norm());
}

inline void Quaternion::Normalize(void)
{
    float fNorm = Norm();
    if ( fNorm > 0.0f )
    {
        float fInvNorm = 1.0f / fNorm;
        w *= fInvNorm;
        x *= fInvNorm;
        y *= fInvNorm;
        z *= fInvNorm;
    }
}

inline float Quaternion::Norm(void) const
{
    return w*w + x*x + y*y + z*z;
}

inline Quaternion Quaternion::Inverse (void) const
{
    float fNorm = Norm();

    if ( fNorm > 0.0f )
    {
        float fInvNorm = 1.0f / fNorm;
        return Quaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
    }
    else
    {
        // return an invalid result to flag the error
        return ZERO;
    }
}

inline Quaternion Quaternion::UnitInverse (void) const
{
   // this must be unit length.
   return Quaternion (w,-x,-y,-z);
}

#endif
