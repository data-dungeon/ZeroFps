#include "zfassert.h"
#include "plane.h"

// Constructors
Plane::Plane(const Plane& kPlane)
{
	m_kNormal	= kPlane.m_kNormal;
	m_fD		= kPlane.m_fD;
	
}

Plane::Plane(Vector3& kPoint0, Vector3& kPoint1, Vector3& kPoint2)
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
void Plane::Set(Vector3 kNormal, Vector3 kPos)
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

bool Plane::PointInside(Vector3 kPoint)
{
	return ((m_kNormal.Dot(kPoint) + m_fD) >= 0);
}

bool Plane::SphereInside(Vector3 kCenter, float fRadius)
{
	float d = m_kNormal.Dot(kCenter) + m_fD;
	if( d <= -fRadius )
		return false;

	return true;
}


float Plane::PointTest(Vector3 kPoint)
{
	return (m_kNormal.Dot(kPoint) + m_fD);
}

float Plane::SphereTest(Vector3 kPoint,float fRadius)
{
	return (m_kNormal.Dot(kPoint) + m_fD) - fRadius;
}

bool Plane::LineTest(Vector3 kP1,Vector3 kP2,Vector3* kColPos)
{
//	float i0=(kNormal.x*kP1.x)+(kNormal.y*kP1.y)+(kNormal.z*kP1.z);
//	float i1=(kNormal.x*kP2.x)+(kNormal.y*kP2.y)+(kNormal.z*kP2.z);	
	float i0=m_kNormal.Dot(kP1);
	float i1=m_kNormal.Dot(kP2);


	float final_t= -(i1 + m_fD) / (i0-i1);
	
	

	
//	pos.x = (kP1.x*final_t) + ( kP2.x* (1-final_t));
//	pos.y = (kP1.y*final_t) + ( kP2.y* (1-final_t));
//	pos.z = (kP1.z*final_t) + ( kP2.z* (1-final_t));	
	
	if(final_t >= 0 && final_t <= 1){
		*kColPos = (kP1*final_t) + (kP2*(1 - final_t));		
		return true;
	}
	else
		return false;
}

void Plane::Set(Vector3& kPoint0, Vector3& kPoint1, Vector3& kPoint2)
{
	Vector3 kDiff1 = kPoint1 - kPoint0;
	Vector3 kDiff2 = kPoint2 - kPoint0;

    m_kNormal = kDiff1.Cross(kDiff2);
    m_kNormal.Normalize();

    m_fD = - kPoint0.Dot( m_kNormal );
}


float Plane::SolveY(float X, float Z)
{
	if (m_kNormal.y)
		return ( -(m_kNormal.x * X + m_kNormal.z * Z + m_fD) / m_kNormal.y );

	return (0.0f);
}