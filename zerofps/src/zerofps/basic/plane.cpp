#include "zfassert.h"
#include "plane.h"


void Plane::Set(Vector3 kNormal, Vector3 kPos)
{
	m_kNormal=kNormal;

//	cout<<"angle:"<<kPos.Dot(kNormal)<<endl;
	if(kPos.Dot(kNormal) >0)
	{
//		cout<<"negativ"<<endl;
		m_fD = -(kNormal.Proj(kPos).Length());
	}
	else {
//		cout<<"possitiv"<<endl;
		m_fD = kNormal.Proj(kPos).Length();	
	}
	
//	cout<<"Distance:"<<m_fD<<endl;

}

bool Plane::PointInside(Vector3 kPoint)
{
	return ((m_kNormal.Dot(kPoint) + m_fD) <= 0);
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
	
	if(final_t > 0 && final_t < 1){
		*kColPos = (kP1*final_t) + (kP2*(1 - final_t));		
		return true;
	}
	else
		return false;
}

