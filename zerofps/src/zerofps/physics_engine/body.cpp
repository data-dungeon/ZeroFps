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
	m_kInertia.m_afData[0] = 0.5;
	m_kInertia.m_afData[4] = 0.5;
	m_kInertia.m_afData[8] = 0.5;
	
	m_kInertia.inverse(m_kInertiaInverse,0.1);
//	m_kInertiaInverse 
	m_kMassCenter.Set(0,0,0);
	
	m_kPosition.Set(0,0,0);
	m_kVelocity.Set(0,0,0);
	m_kBodyVelocity.Set(0,0,0);	
	m_kAcceleration.Set(0,0,0);
	
	m_kAngles.Set(0,0,0);
	m_kAngleVel.Set(0,0,0);
	m_kAngleAcceleration.Set(0,0,0);									
	m_kOrientation.Set(0,0,0,0);
	
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

Vector3 Body::SetRot(Vector3 kRot)
{
	m_kAngles = kRot;
	m_kOrientation.FromAxes(&kRot); 
}

Vector3 Body::GetRot()
{
	return m_kAngles;
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
	Matrix3 rot;
	m_kOrientation.ToRotationMatrix(rot);


	Vector3 temp = kVert;
	
	kVert *=(*m_pfScale);	
	kVert = rot.VectorTransform(kVert);
	kVert += m_kPosition;

	return kVert;

}


bool Body::SetMad(Mad_Core* pkMad,int iMesh)
{
	Mad_CoreMesh* pkCoreMech;

	//look for mech pointer in core
	pkCoreMech = GetMeshByID(pkMad,iMesh);					
	if(pkCoreMech != NULL)		
	{
		cout<<"found mech"<<endl;
		
		m_pkFaces = &pkCoreMech->akFaces;
		m_pkVertex = &pkCoreMech->akFrames[0].akVertex;
		m_pkNormal = &pkCoreMech->akFrames[0].akNormal;
		
		m_fRadius = pkMad->m_fBoundRadius;
		
		//found the mech return true
		m_bPolygonCheck = true;
		return true;
	}
	
	return false;
}

Mad_CoreMesh* Body::GetMeshByID(Mad_Core* pkMad,int iMesh)
{
	if(iMesh < 0)	
		return NULL;
	if(iMesh >= pkMad->m_kMesh.size())
		return NULL;

	return &pkMad->m_kMesh[iMesh];
}



