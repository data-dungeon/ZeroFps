#ifndef _BASIC_PLANE_H_
#define _BASIC_PLANE_H_

#include "vector3.h"
#include "basic_x.h"

/**	\brief	Descibes a Plane in 3D.
		\ingroup Basic
*/
class BASIC_API Plane 
{
	public:
// Std Values

// Data
	Vector3	m_kNormal;							///< Normal of plane ( A,B,C in plane equation).
	float		m_fD;									///< Distance ( D in plane equation).

// Constructors
	Plane() { };
	Plane(const Plane& kPlane);
   Plane(const Vector3& kPoint0, const Vector3& kPoint1, const Vector3& kPoint2);

// Access 
	void SetNormal(Vector3& kVec);
	Vector3 GetNormal();
	void SetDist(float fDist);
	float GetDist();

// Assignment 
	Plane& operator=( const Plane& kIn );
	void Set(const float& fX, const float& fY, const float& fZ, const float& fInD) ;
	void Set(const Vector3& kNormal,const float& fInD);
	void Set(const Vector3& kPoint0, const Vector3& kPoint1, const Vector3& kPoint2);
	void Set(const Vector3& kNormal, const Vector3& kPos);
	void Zero();

// Comparison
	bool Compare(const Plane& kP) const;
	bool Compare(const Plane& kP, const float e ) const;		///< True if this is == v within the range of +-e
	bool operator==(const Plane& kP)	const;
	bool operator!=(const Plane& kP)	const;
	
// Arithmetic 
// Geometric

// Other
	string ToString();

// NoSort
	bool PointInside(const Vector3& kPoint);
	bool SphereInside(const Vector3& kCenter, const float& fRadius);
	
	float SphereTest(const Vector3& kPoint,const float& fRadius);
	float PointTest(const Vector3& kPoint);
	bool LineTest(const Vector3& kP1,const Vector3& kP2,Vector3* kColPos); 

	float SolveY(const float& X, const float& Z);
	Vector3 ProjectOnto(Vector3 kVec);
};

// Constructors
inline Plane::Plane(const Plane& kPlane)
{
	m_kNormal	= kPlane.m_kNormal;
	m_fD		= kPlane.m_fD;
	
}

inline Plane::Plane(const Vector3& kPoint0, const Vector3& kPoint1, const Vector3& kPoint2)
{
	Set(kPoint0,kPoint1,kPoint2);
}

// Access 
inline void Plane::SetNormal(Vector3& kVec)
{
	m_kNormal = kVec;
}

inline Vector3 Plane::GetNormal()
{
	return m_kNormal;	
}

inline void Plane::SetDist(float fDist)
{
	m_fD = fDist;
}

inline float Plane::GetDist()
{
	return m_fD;
}

// Assignment 
inline Plane& Plane::operator=( const Plane& kIn )
{
	m_kNormal	= kIn.m_kNormal;
	m_fD		= kIn.m_fD;
	return (*this);
}

inline void Plane::Set(const float& fX, const float& fY, const float& fZ, const float& fInD) 
{
	m_kNormal.Set(fX,fY,fZ);
	m_fD = fInD;
}

inline void Plane::Set(const Vector3& kNormal,const float& fInD) 
{
	m_kNormal=kNormal;
	m_fD = fInD;
}	

inline void Plane::Set(const Vector3& kPoint0, const Vector3& kPoint1, const Vector3& kPoint2)
{
	static Vector3 kDiff1;
	static Vector3 kDiff2;

	kDiff1 = kPoint1 - kPoint0;
	kDiff2 = kPoint2 - kPoint0;

	m_kNormal = kDiff1.Cross(kDiff2);
	m_kNormal.Normalize();

    m_fD = - kPoint0.Dot( m_kNormal );
}

inline void Plane::Set( const Vector3& kNormal, const Vector3& kPos)
{
	m_kNormal=kNormal;

	if(kPos.Dot(kNormal) >0)
	{
		m_fD = -(kNormal.Proj(kPos).Length());
	}
	else {
		m_fD = kNormal.Proj(kPos).Length();	
	}
}

inline void Plane::Zero()
{
	m_kNormal.Zero();
	m_fD = 0;
}

// Comparison
inline bool Plane::Compare(const Plane& kP) const
{
	return (m_kNormal == kP.m_kNormal && m_fD == kP.m_fD);
}

inline bool Plane::Compare(const Plane& kP, const float e ) const
{
	return (fabs(m_kNormal.x-kP.m_kNormal.x) < e && fabs(m_kNormal.y-kP.m_kNormal.y) < e && fabs(m_kNormal.z-kP.m_kNormal.z) < e && fabs(m_fD-kP.m_fD));		
}

inline bool Plane::operator==(const Plane& kP) const
{
	return Compare(kP);	
}

inline bool Plane::operator!=(const Plane& kP) const
{
	return !Compare(kP);	
}


#endif






