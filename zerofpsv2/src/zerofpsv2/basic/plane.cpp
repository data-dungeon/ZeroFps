#include "zfassert.h"
#include "plane.h"

// Constructors
Plane::Plane(const Plane& kPlane)
{
	m_kNormal	= kPlane.m_kNormal;
	m_fD		= kPlane.m_fD;
	
}

Plane::Plane(const Vector3& kPoint0, const Vector3& kPoint1, const Vector3& kPoint2)
{
	Set(kPoint0,kPoint1,kPoint2);
}


// Operators
Plane& Plane::operator=( const Plane& kIn )
{
	m_kNormal	= kIn.m_kNormal;
	m_fD		= kIn.m_fD;
	return (*this);
}

// Methods
void Plane::Set( Vector3 kNormal, const Vector3& kPos)
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

bool Plane::PointInside(const Vector3& kPoint)
{
	return ((m_kNormal.Dot(kPoint) + m_fD) >= 0);
}

bool Plane::SphereInside(const Vector3& kCenter, const float& fRadius)
{
	float d = m_kNormal.Dot(kCenter) + m_fD;
	if( d <= -fRadius )
		return false;

	return true;
}


float Plane::PointTest(const Vector3& kPoint)
{
	return (m_kNormal.Dot(kPoint) + m_fD);
}

float Plane::SphereTest(const Vector3& kPoint,const float& fRadius)
{
	return (m_kNormal.Dot(kPoint) + m_fD) - fRadius;
}

bool Plane::LineTest(const Vector3& kP1,const Vector3& kP2,Vector3* kColPos)
{
	if( ((PointTest(kP1) > 0) && (PointTest(kP2) < 0)) ||
		 ((PointTest(kP1) < 0) && (PointTest(kP2) > 0)) )
	{

		float i0=m_kNormal.Dot(kP1);
		float i1=m_kNormal.Dot(kP2);
		
		float final_t= -(i1 + m_fD) / (i0-i1);
		
		
		if(final_t >= 0 && final_t <= 1)
		{
			*kColPos = (kP1*final_t) + (kP2*(1 - final_t));		
			return true;
		}
		else
			return false;
	
	}
	else
	{
		return false;
	}

}

void Plane::Set(const Vector3& kPoint0, const Vector3& kPoint1, const Vector3& kPoint2)
{
	static Vector3 kDiff1;
	static Vector3 kDiff2;

	kDiff1 = kPoint1 - kPoint0;
	kDiff2 = kPoint2 - kPoint0;

	m_kNormal = kDiff1.Cross(kDiff2);
	m_kNormal.Normalize();

    m_fD = - kPoint0.Dot( m_kNormal );
}


float Plane::SolveY(const float& X, const float& Z)
{
	if (m_kNormal.y)
		return ( -(m_kNormal.x * X + m_kNormal.z * Z + m_fD) / m_kNormal.y );

	return (0.0f);
}














