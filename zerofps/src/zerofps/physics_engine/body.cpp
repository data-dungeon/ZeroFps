#include "body.h"

Body::Body()
{
	
	Reset();
}


void Body::Reset()
{
	m_bPolygonCheck =		false;
	m_bSolid	=				true;
	m_bGravity =			true;
	m_bResting =			false;
	m_fBounce =				0.8;
	
	
	m_pkFaces =				NULL;
	m_pkVertex =			NULL;
	m_pkNormal = 			NULL;
	
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
	m_pfScale =				NULL;

	m_kVertex[0].Set(-1,0,0);
	m_kVertex[1].Set(1,-1,0);	
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

Vector3 Body::TransRot(Vector3 kVert)
{
	return (kVert * (*m_pfScale)) + m_kPosition;

}


bool Body::SetMad(Mad_Core* pkMad,int iMesh)
{
	Mad_CoreMesh* pkCoreMech;

	//look for mech pointer in core
	pkCoreMech = pkMad->GetMeshByID(iMesh);					
	if(pkCoreMech != NULL)		
	{
		cout<<"found mech"<<endl;
		
		m_pkFaces = pkCoreMech->GetFacesPointer();
		m_pkVertex = (*pkCoreMech->GetVertexFramePointer())[0].GetVertexPointer();
		m_pkNormal = (*pkCoreMech->GetVertexFramePointer())[0].GetNormalPointer();
		
		m_fRadius = pkMad->GetRadius();
		
		//found the mech return true
		m_bPolygonCheck = true;
		return true;
	}
	
	return false;
}





