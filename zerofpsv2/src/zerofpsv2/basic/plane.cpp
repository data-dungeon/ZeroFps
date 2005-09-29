#include "zfassert.h"
#include "plane.h"

// Methods
Vector3 Plane::ProjectOnto(Vector3 kVec)
{
	float fDist = m_kNormal.Dot(kVec) + m_fD;
	Vector3 kNewVec = kVec * fDist;
	return kNewVec;
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

float Plane::SolveY(const float& X, const float& Z)
{
	if (m_kNormal.y)
		return ( -(m_kNormal.x * X + m_kNormal.z * Z + m_fD) / m_kNormal.y );

	return (0.0f);
}

string Plane::ToString()
{
	char szVec[128];
	sprintf(szVec, "%f,%f,%f,%f", m_kNormal.x,m_kNormal.y,m_kNormal.z, m_fD);
	return string(szVec);	
}












