#include "body.h"

Body::Body()
{
	
	Reset();
}
 

void Body::Reset()
{
	m_bPolygonCheck =		false;
	m_bSolid	=				false;
	m_bGravity =			true;
	m_bResting =			false;
	m_fBounce =				0.9;
	m_fAirFriction =		0.001;
	
	m_pkFaces =				NULL;
	m_pkVertex =			NULL;
	m_pkNormal = 			NULL;
	
	m_fMass =				1;
	m_kInertia.Identity();
	m_kInertia.m_afData[0] = 1;
	m_kInertia.m_afData[4] = 1;
	m_kInertia.m_afData[8] = 1;
	
	
	m_kInertia.Inverse(m_kInertiaInverse,0.1);
	
	m_kInertiaInverse*=10;
	
	m_kMassCenter.Set(0,0,0);	
	m_kPosition.Set(0,0,0);
	m_kVelocity.Set(0,0,0);
	m_kBodyVelocity.Set(0,0,0);	
	m_kAcceleration.Set(0,0,0);
	
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

void Body::SetRot(Matrix3 kRot)
{
	m_kOrientation = kRot;
}

Matrix3 Body::GetRot()
{
	return m_kOrientation;
}



void Body::Rest(Body* pkBody)
{
	m_bResting = true;	
	m_kVelocity.Set(0,0,0);
	m_kAcceleration.Set(0,0,0);
	m_kForces.Set(0,0,0);
	m_kAngleVel.Set(0,0,0);
//	cout<<"Object going to sleep"<<endl;
}

void Body::Awaken()
{ 
	m_bResting = false;
//	cout<<"Objects Awakens"<<endl;

}

Vector3 Body::TransRot(Vector3 kVert)
{
	Vector3 temp = kVert;
	
	kVert *=(*m_pfScale);	
	kVert = m_kOrientation.VectorTransform(kVert);
	kVert += m_kPosition;

	return kVert;

}

Vector3 Body::Rot(Vector3 kVert)
{
	Vector3 temp = kVert;
	
	kVert *=(*m_pfScale);	
	kVert = m_kOrientation.VectorTransform(kVert);

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
		
		//get radius
		m_fRadius = pkMad->m_fBoundRadius * (*m_pfScale);
		
		//calculate mass center		
		m_kMassCenter = CalculateMassCenter();
		//m_fRadius = CalculateRadius();
		
		return true;
	}
	
	return false;
}

Mad_CoreMesh* Body::GetMeshByID(Mad_Core* pkMad,int iMesh)
{
	if(iMesh < 0)	
		return NULL;
	if(iMesh >= int(pkMad->m_kMesh.size()))
		return NULL;

	return &pkMad->m_kMesh[iMesh];
}




Vector3 Body::CalculateMassCenter()
{
	Vector3 mc = Vector3(0,0,0);	
	if(m_pkVertex)
	{
		Vector3 max = Vector3(0,0,0);
		Vector3 min = Vector3(0,0,0);
	
		for(int i = 0;i<m_pkVertex->size();i++)
		{
			if((*m_pkVertex)[i].x > max.x)
				max.x = (*m_pkVertex)[i].x;			
			if((*m_pkVertex)[i].y > max.y)
				max.y = (*m_pkVertex)[i].y;
			if((*m_pkVertex)[i].z > max.z)
				max.z = (*m_pkVertex)[i].z;
		
			if((*m_pkVertex)[i].x < min.x)
				min.x = (*m_pkVertex)[i].x;			
			if((*m_pkVertex)[i].y < min.y)
				min.y = (*m_pkVertex)[i].y;
			if((*m_pkVertex)[i].z < min.z)
				min.z = (*m_pkVertex)[i].z;
		
		}	

		mc = (max + min) /2;
	
/*		for(int i = 0;i<m_pkVertex->size();i++)
		{
			mc +=	(*m_pkVertex)[i];		
		}	
		
		mc = mc / m_pkVertex->size();
*/		
	}
	
	return mc;
}

float Body::CalculateRadius()
{
	float fRadius = 1;
	
	if(m_pkVertex)
	{
		Vector3 max = Vector3(0,0,0);
		Vector3 min = Vector3(0,0,0);
	
		for(int i = 0;i<m_pkVertex->size();i++)
		{
			if((*m_pkVertex)[i].x > max.x)
				max.x = (*m_pkVertex)[i].x;			
			if((*m_pkVertex)[i].y > max.y)
				max.y = (*m_pkVertex)[i].y;
			if((*m_pkVertex)[i].z > max.z)
				max.z = (*m_pkVertex)[i].z;
		
			if((*m_pkVertex)[i].x < min.x)
				min.x = (*m_pkVertex)[i].x;			
			if((*m_pkVertex)[i].y < min.y)
				min.y = (*m_pkVertex)[i].y;
			if((*m_pkVertex)[i].z < min.z)
				min.z = (*m_pkVertex)[i].z;
		
		}	

		fRadius = (max - min).Length()/2;
	}
	
	return fRadius;
}
