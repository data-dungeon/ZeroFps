#include "body.h"

Body::Body()
{
	
	Reset();
}


void Body::Reset()
{
	m_pkMad =				NULL;
	m_bPolygonCheck =		false;
	m_bSolid	=				true;
	m_bGravity =			true;
	m_bResting =			false;
	m_fBounce =				0.8;
	
	
	m_fMass =				1;
	m_kInertia.Identity();
	m_kInertiaInverse.Identity();
	m_kMassCenter.Set(0,0,0);
	
	m_kPosition.Set(0,0,0);
	m_kVelocity.Set(0,0,0);
	m_kBodyVelocity.Set(0,0,0);	
	m_kAcceleration.Set(0,0,0);
	
	m_kAngles.Identity();
	m_kAngleVel.Set(0,0,0);
	m_kAngleAcceleration.Set(0,0,0);									
	m_kOrientation.Identity();
	
	m_kForces.Set(0,0,0);
	m_kMoment.Set(0,0,0);
	
	m_kIeInverse.Identity();
	
	m_fRadius =				0.5;
	
}


void Body::SetPos(Vector3 kPos)
{
	m_kPosition = kPos;

}

Vector3 Body::GetPos()
{
	return m_kPosition;
}


void Body::Rest(Body* pkBody)
{
	m_bResting = true;	
	m_kVelocity.Set(0,0,0);
	m_kAcceleration.Set(0,0,0);
	m_kForces.Set(0,0,0);
	
//	cout<<"Object going to sleep"<<endl;
}

void Body::Awaken()
{
	m_bResting = false;
//	cout<<"Objects Awakens"<<endl;

}






