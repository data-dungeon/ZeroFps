#include "physics_engine.h"
#include "../render/render.h"

Physics_Engine::Physics_Engine() 
: ZFSubSystem("Physics_Engine")
{
	m_fCtol		= 0.01;
	m_iMaxTests = 20;
	
}

bool Physics_Engine::StartUp()
{ 
	Clear();
	return true; 
}

bool Physics_Engine::ShutDown()  
{ 
	return true; 
}

bool Physics_Engine::IsValid()	{ return true; }

 
void Physics_Engine::Clear()
{
	m_kBodys.clear();
}

void Physics_Engine::Update(float fAlphaTime)
{	
	if(m_kBodys.size() == 0)
		return;

	m_fAlphaTime = fAlphaTime;
	m_fRTime = m_fAlphaTime;
	
	//make sure there is no collissions	
	m_kCollissions.clear();	
	m_iNrOfCollissions=0;

	UpdateForces();	
	while(m_fRTime > 0)
	{
		TestWithPlanes(m_fRTime);
		TestBodys(m_fRTime);
		
		if(m_kCollissions.size() > 0 )
		{
			m_iNrOfCollissions++;	
		
			//get next closest collission	
			Collission* pkCol = FindNextCollission();
			
			//update all objects up to current time
			UpdateVelNPos(pkCol->fAtime);			
			
			//handle collission
			HandleCollission(pkCol);
			
			//dont shrink rest time with to little not good for FPS =)
			if(pkCol->fAtime < 0.001)
				pkCol->fAtime = 0.001;
					
			//set new Resttime			
			m_fRTime -= pkCol->fAtime;			
			
			//clear all collissions
			m_kCollissions.clear();
		}
		else 
		{
			UpdateVelNPos(m_fRTime);	
			m_fRTime = 0;
		}
	}


	//draw debug graph
	static Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));	
	for(list<Body*>::iterator it = m_kBodys.begin(); it != m_kBodys.end(); it++) 
	{
		// Vim - is this the wrong way?
		// pkRender->Sphere((*it)->m_kPosition,(*it)->m_fRadius*2,1,Vector3(0,1,0),false);
		// pkRender->Sphere((*it)->m_kPosition + (*it)->Rot((*it)->m_kMassCenter),0.1,1,Vector3(0,1,1),false);		
	
	}


//	if(m_iNrOfCollissions >0)
//		cout<<"TOTAL NR OF COLLISSIONS: "<<m_iNrOfCollissions<<endl;
}

bool Physics_Engine::AddBody(Body* pkBody)
{
	if(ExistBody(pkBody))
		return false;

	m_kBodys.push_back(pkBody);
	
	cout<<"Body Added to physics engine"<<endl;
	
	return true;
}

bool Physics_Engine::RemoveBody(Body* pkBody)
{
	m_kBodys.remove(pkBody);
	
	cout<<"Body removed from physics engine"<<endl;
	return true;
}

bool Physics_Engine::ExistBody(Body* pkBody)
{
	for(list<Body*>::iterator it = m_kBodys.begin(); it != m_kBodys.end(); it++) 
	{
		if((*it) == pkBody)
			return true;
	}
	
	return false;
}

bool Physics_Engine::AddPlane(Plane* pkPlane)
{
	if(ExistPlane(pkPlane))
		return false;

	m_kPlanes.push_back(pkPlane);
	
	cout<<"Plane Added to physics engine"<<endl;
	
	return true;
}

bool Physics_Engine::RemovePlane(Plane* pkPlane)
{
	m_kPlanes.remove(pkPlane);
	
	cout<<"Plane removed from physics engine"<<endl;
	return true;
}

bool Physics_Engine::ExistPlane(Plane* pkPlane)
{
	for(list<Plane*>::iterator it = m_kPlanes.begin(); it != m_kPlanes.end(); it++) 
	{
		if((*it) == pkPlane)
			return true;
	}
	
	return false;	
}

void Physics_Engine::UpdateForces()
{
	for(list<Body*>::iterator it = m_kBodys.begin(); it != m_kBodys.end(); it++) 
	{
		(*it)->m_kForces.Set(0,0,0);
		(*it)->m_kMoment.Set(0,0,0);		
	
		if(!(*it)->m_bResting) 
		{
			if((*it)->m_bGravity)
				(*it)->m_kForces += Vector3(0,-10,0);
				
		}
	}
}

void Physics_Engine::UpdateVelNPos(float fAtime)
{
	for(list<Body*>::iterator it = m_kBodys.begin(); it != m_kBodys.end(); it++) 
	{
		if(!(*it)->m_bResting)
			UpdateBodyVelNPos(*it,fAtime);
	}
}

void Physics_Engine::UpdateBodyVelNPos(Body* pkBody,float fAtime)
{
		//position

		Vector3 kAe;
			
		//Calculate acceleration in world space
		kAe = pkBody->m_kForces / pkBody->m_fMass;
		pkBody->m_kAcceleration = kAe;
		
		//Calculate velocity in world space
		pkBody->m_kVelocity += kAe * fAtime;
		
		//Calculate position in world space
		pkBody->m_kPosition += pkBody->m_kVelocity * fAtime;
		
		

		//rotation
		
		//calculate angular acceleration
		pkBody->m_kAngleAcceleration = pkBody->m_kInertiaInverse * (pkBody->m_kMoment - pkBody->m_kAngleVel.Cross(pkBody->m_kInertia*pkBody->m_kAngleVel) );
	
		//calculate new rotation velocity
		pkBody->m_kAngleVel += pkBody->m_kAngleAcceleration * fAtime;
		
		//apply air friction
		pkBody->m_kAngleVel-= (pkBody->m_kAngleVel*pkBody->m_fAirFriction);
		
		//calculate new orientation		
		pkBody->m_kOrientation.Rotate(pkBody->m_kAngleVel*fAtime);

		//calculate velocity in body cordinats
		pkBody->m_kBodyVelocity = pkBody->m_kOrientation.VectorTransform(pkBody->m_kVelocity);
		
		
/*		//calculate body speed
		pkBody->m_fSpeed = pkBody->m_kVelocity.Length();
*/		
		//get euler angles
//		pkBody->m_kOrientation.ToAxes(&pkBody->m_kAngles);

}

void Physics_Engine::TestWithPlanes(float fATime)
{
	for(list<Plane*>::iterator planeit = m_kPlanes.begin(); planeit != m_kPlanes.end(); planeit++) 
	{
		for(list<Body*>::iterator bodyit = m_kBodys.begin(); bodyit != m_kBodys.end(); bodyit++) 
		{
			if(!(*bodyit)->m_bResting)
				if((*bodyit)->m_bPolygonCheck)
					TestMeshVSPlane(*bodyit,*planeit,fATime);
				else
					TestSphereVSPlane(*bodyit,*planeit,fATime);
		}
	}
}

void Physics_Engine::TestBodys(float fATime)
{
	for(list<Body*>::iterator bodyit1 = m_kBodys.begin(); bodyit1 != m_kBodys.end(); bodyit1++) 
	{		
		for(list<Body*>::iterator bodyit2 = bodyit1; bodyit2 != m_kBodys.end(); bodyit2++) 
		{
			if( (*bodyit1) == (*bodyit2)) 
				continue;
			
			if( (*bodyit1)->m_bResting && (*bodyit2)->m_bResting)
				continue;
			
			if((*bodyit1)->m_bPolygonCheck && (*bodyit2)->m_bPolygonCheck)
				TestBodyMeshVSBodyMesh(*bodyit1,*bodyit2,fATime);
			else
				TestBodyVSBody(*bodyit1,*bodyit2,fATime);
		}
	}
}

bool Physics_Engine::TestSphereVSPlane(Body* pkBody,Plane* pkPlane,float fATime)
{
	float atime = fATime;	
	float dtime = atime;
	bool retry = true;
	bool didpen = false;
	bool collission = false;
	int nroftests = 0;
	
	
	while(retry && nroftests < m_iMaxTests)
	{
		retry = false;
		nroftests++;
		
		memcpy(&m_kBodyCopy1,pkBody,sizeof(Body));		
		UpdateBodyVelNPos(&m_kBodyCopy1,atime);
	
		int check = CollideSphereVSPlane(&m_kBodyCopy1,pkPlane);			
		
		if(check == PENETRATING || check == COLLISSION)
		{
			//is it the first penetration in test?
			if(didpen != true)
			{
				memcpy(&m_kBodyCopy1,pkBody,sizeof(Body));		
		
				int check2 = CollideSphereVSPlane(&m_kBodyCopy1,pkPlane);			
			
				//check if there still is a penetration if atime is 0 	
				if(check2 == PENETRATING || check2 == COLLISSION)
				{
					atime = 0;
					didpen = true;
					break;
				}
			}
		
			didpen = true;			
			retry = true;
			atime /=1.5;
			
			continue;
		}
		
		if(check == NOT && didpen)
		{
			break;
		}
		
		if(check == NOT && didpen == false)
			return false;
		
		//if time is infinit short set it to 0	
		if(atime < 0.0001)
		{	
			atime =0;
			break;
		}	

	}
	
	if(didpen)
	{	
		Collission tempcol;

		tempcol.pkBody1 = pkBody;
		tempcol.pkBody2 = NULL;
		
		tempcol.kPos = m_kBodyCopy1.m_kPosition - (pkPlane->m_kNormal * m_kBodyCopy1.m_fRadius);
		tempcol.kMassCenterToPos1 = (tempcol.pkBody1->m_kPosition + tempcol.pkBody1->m_kMassCenter) - tempcol.kPos;
		tempcol.kMassCenterToPos2 = Vector3(0,0,0);
		tempcol.kNormal = pkPlane->m_kNormal;	
		tempcol.kRelVelocity = m_kBodyCopy1.m_kVelocity;
		tempcol.kRelAcceleration = m_kBodyCopy1.m_kAcceleration;
		tempcol.fAtime =	atime;	
		tempcol.kCollissionTangent = (tempcol.kNormal.Cross(tempcol.kRelVelocity.Unit())).Cross(tempcol.kNormal);
	
		m_kCollissions.push_back(tempcol);	
	
		return true;
	}
	
	return false;
}

bool Physics_Engine::TestMeshVSPlane(Body* pkBody,Plane* pkPlane,float fATime)
{
	float atime = fATime;	
	float dtime = atime;
	bool retry = true;
	bool didpen = false;
	bool collission = false;
	int nroftests = 0;
	
	
	while(retry && nroftests < m_iMaxTests)
	{
		retry = false;
		nroftests++;
		
		memcpy(&m_kBodyCopy1,pkBody,sizeof(Body));		
		UpdateBodyVelNPos(&m_kBodyCopy1,atime);
	
		int check = NOT;
		if(CollideSphereVSPlane(&m_kBodyCopy1,pkPlane) != NOT)
			check = CollideMeshVSPlane(&m_kBodyCopy1,pkPlane);			
		
		
		if(check == PENETRATING || check == COLLISSION)
		{
			didpen = true;			
			retry = true;
			atime /=2;
			
			continue;
		}
		
		if(check == NOT && didpen)
		{
			break;
		}
		
		if(check == NOT && didpen == false)
			return false;
		
	}
	
	if(didpen)
	{	
		Vector3 kMassCenter = pkBody->Rot(pkBody->m_kMassCenter);
		
		//add all temp collissions
		for(unsigned int i=0;i<m_kCollissionPoints.size();i++)
		{
		
			Collission tempcol;

			tempcol.pkBody1 = pkBody;
			tempcol.pkBody2 = NULL;
		

			tempcol.kPos = m_kBodyCopy1.TransRot((*m_kBodyCopy1.m_pkVertex)[m_kCollissionPoints[i]]);						
			tempcol.kMassCenterToPos1 = (tempcol.pkBody1->m_kPosition + kMassCenter)- tempcol.kPos;
			tempcol.kMassCenterToPos2.Set(0,0,0);
			tempcol.kNormal = pkPlane->m_kNormal;	
			tempcol.kRelVelocity = m_kBodyCopy1.m_kVelocity;
			tempcol.kRelAcceleration = m_kBodyCopy1.m_kAcceleration;
			tempcol.fAtime =	atime;	
			tempcol.kCollissionTangent = (tempcol.kNormal.Cross(tempcol.kRelVelocity.Unit())).Cross(tempcol.kNormal);
	
			m_kCollissions.push_back(tempcol);	
		}
		return true;
	}
	
	return false;
}



int Physics_Engine::CollideSphereVSPlane(Body* pkBody,Plane* pkPlane)
{
	float d = pkPlane->SphereTest(pkBody->m_kPosition,pkBody->m_fRadius);
		
	if(fabs(d) <= m_fCtol)	
		return COLLISSION;
	
	if(d < 0)
		return PENETRATING;
				
	return NOT;
}

bool Physics_Engine::TestBodyMeshVSBodyMesh(Body* pkBody1,Body* pkBody2,float fATime)
{
	
	float atime = fATime;	
	float dtime = atime;
	bool retry = true;
	bool didpen = false;
	bool collission = false;
	int nroftests = 0;
	
	while(retry && nroftests < m_iMaxTests)
	{
		retry = false;
		nroftests++;
		
		memcpy(&m_kBodyCopy1,pkBody1,sizeof(Body));
		memcpy(&m_kBodyCopy2,pkBody2,sizeof(Body));
		
		UpdateBodyVelNPos(&m_kBodyCopy1,atime);
		UpdateBodyVelNPos(&m_kBodyCopy2,atime);	
	
		int check = NOT;
		if(CollideBody(&m_kBodyCopy1,&m_kBodyCopy2) != NOT)
			check = CollideBodyMeshs(&m_kBodyCopy1,&m_kBodyCopy2);			
		
		
		if(check == PENETRATING || check == COLLISSION)
		{
			didpen = true;			
			retry = true;
			atime /=2;
			
			continue;
		}
		
		if(check == NOT && didpen)
		{
			break;
		}
		
		if(check == NOT && didpen == false)
			return false;
	}	
	
	if(didpen)
	{	
		Vector3 kMassCenter1 = pkBody1->Rot(pkBody1->m_kMassCenter);
		Vector3 kMassCenter2 = pkBody2->Rot(pkBody2->m_kMassCenter);		
		
		//add all temp collissions
		for(unsigned int i=0;i<m_kCollissionCords.size();i++)
		{
		
		
			Collission tempcol;

			tempcol.pkBody1 = pkBody1;
			tempcol.pkBody2 = pkBody2;
		

			tempcol.kPos = m_kCollissionCords[i];						
			tempcol.kMassCenterToPos1 = (tempcol.pkBody1->m_kPosition + kMassCenter1)- tempcol.kPos;
			tempcol.kMassCenterToPos2 = (tempcol.pkBody2->m_kPosition + kMassCenter2)- tempcol.kPos;
			tempcol.kNormal = (m_kBodyCopy1.m_kPosition - m_kBodyCopy2.m_kPosition).Unit();	
			tempcol.kRelVelocity = m_kBodyCopy1.m_kVelocity - m_kBodyCopy2.m_kVelocity;
			tempcol.kRelAcceleration = m_kBodyCopy1.m_kAcceleration - m_kBodyCopy2.m_kAcceleration;
			tempcol.fAtime =	atime;	
			tempcol.kCollissionTangent = (tempcol.kNormal.Cross(tempcol.kRelVelocity.Unit())).Cross(tempcol.kNormal);
	
			m_kCollissions.push_back(tempcol);	
		}
		return true;
	}

	return false; // Zeblar: Vet inte vad den här funktionen skall returnera men jag chansar på FALSE :)
}

int Physics_Engine::CollideBodyMeshs(Body* pkBody1,Body* pkBody2)
{
	if(pkBody1->m_pkVertex == NULL)
		return NOT;
	
	if(pkBody2->m_pkVertex == NULL)
		return NOT;
	
	
	bool clear=false;
	

	static Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));
		
	for(unsigned int i=0;i<pkBody1->m_pkFaces->size();i++)	
	{
		//get vertexes
		Vector3 point[3]; 
		point[0] = pkBody1->TransRot((*pkBody1->m_pkVertex)[(*pkBody1->m_pkFaces)[i].iIndex[0]]);
		point[1] = pkBody1->TransRot((*pkBody1->m_pkVertex)[(*pkBody1->m_pkFaces)[i].iIndex[1]]);		
		point[2] = pkBody1->TransRot((*pkBody1->m_pkVertex)[(*pkBody1->m_pkFaces)[i].iIndex[2]]);		
					
		//get normal		
		Vector3 V1 = point[1] - point[0];
		Vector3 V2 = point[2] - point[0];		
		Vector3 Normal= V1.Cross(V2);		
		Normal.Normalize();
		
		//setup polygon plane
		Plane P;
		P.m_fD = -Normal.Dot(point[0]);	
		P.m_kNormal = Normal;		
		
/*		//debug lines
		pkRender->Line(point[0],point[1]);
		pkRender->Line(point[1],point[2]);		
		pkRender->Line(point[2],point[0]);		
		
		pkRender->Line(point[0],point[0] + Normal);		
		pkRender->Line(point[1],point[1] + Normal);				
		pkRender->Line(point[2],point[2] + Normal);				
*/				
		for(unsigned int j=0;j<pkBody2->m_pkFaces->size();j++)	
		{
			//get vertexes
			Vector3 point2[3]; 
			point2[0] = pkBody2->TransRot((*pkBody2->m_pkVertex)[(*pkBody2->m_pkFaces)[j].iIndex[0]]);
			point2[1] = pkBody2->TransRot((*pkBody2->m_pkVertex)[(*pkBody2->m_pkFaces)[j].iIndex[1]]);		
			point2[2] = pkBody2->TransRot((*pkBody2->m_pkVertex)[(*pkBody2->m_pkFaces)[j].iIndex[2]]);		
				
			
		
			//test against plane
			for(int b = 0 ;b<3;b++)
			{
				int p1,p2;
		
				switch(b)
				{
					case 0:
						p1 = 0;
						p2 = 1;
						break;
					case 1:
						p1 = 1;
						p2 = 2;
						break;
					case 2:
						p1 = 2;
						p2 = 0;
						break;			
				}
		
				//do line test again plane
				Vector3 kColPos;		
				if(P.LineTest(point2[p1], point2[p2],&kColPos))
				{
					//test sides of polygon
					if(TestSides(point,&Normal,kColPos,0))
					{				
						//clear temp collissions if a collission occurs
						if(!clear)
						{	
							m_kCollissionCords.clear();
							clear=true;
						}
						 
						//add point to list
						m_kCollissionCords.push_back(kColPos);
						
					}			
				}
			}
		}
	}
	
	if(clear == true)
		return PENETRATING;
	else
		return NOT;

}



bool Physics_Engine::TestBodyVSBody(Body* pkBody1,Body* pkBody2,float fATime)
{
	
	float atime = fATime;	
	float dtime = atime;
	bool retry = true;
	bool didpen = false;
	bool collission = false;
	int nroftests = 0;
	
	while(retry && nroftests < m_iMaxTests)
	{
		retry = false;
		nroftests++;
		
		memcpy(&m_kBodyCopy1,pkBody1,sizeof(Body));
		memcpy(&m_kBodyCopy2,pkBody2,sizeof(Body));
		
		UpdateBodyVelNPos(&m_kBodyCopy1,atime);
		UpdateBodyVelNPos(&m_kBodyCopy2,atime);	
	
		int check = CollideBody(&m_kBodyCopy1,&m_kBodyCopy2);			
		
		if(check == PENETRATING || check == COLLISSION)
		{
			//is it the first penetration in test?
			if(didpen != true)
			{		
				memcpy(&m_kBodyCopy1,pkBody1,sizeof(Body));
				memcpy(&m_kBodyCopy2,pkBody2,sizeof(Body));
				
				int check2 = CollideBody(&m_kBodyCopy1,&m_kBodyCopy2);			
			
				//check if there still is a penetration if atime is 0 	
				if(check2 == PENETRATING || check2 == COLLISSION)
				{
					atime = 0;
					didpen = true;
					break;
				}
			}
			
			didpen = true;			
			retry = true;
			atime /=1.5;
		
			continue;
		}
		
		if(check == NOT && didpen)
		{
			break;
		}
		
		if(check == NOT && didpen == false)
		{	
			return false;		
		}
			
	}	
	
	if(didpen)
	{	
		
		
		Collission temp;
		temp.pkBody1 = pkBody1;
		temp.pkBody2 = pkBody2;
	
		temp.kNormal = (m_kBodyCopy1.m_kPosition - m_kBodyCopy2.m_kPosition).Unit();
		temp.kPos = m_kBodyCopy1.m_kPosition - (temp.kNormal * m_kBodyCopy1.m_fRadius);	
		temp.kMassCenterToPos1 = (temp.pkBody1->m_kPosition + temp.pkBody1->m_kMassCenter) - temp.kPos;
		temp.kMassCenterToPos2 = (temp.pkBody2->m_kPosition + temp.pkBody2->m_kMassCenter) - temp.kPos;					
		temp.kRelVelocity = m_kBodyCopy1.m_kVelocity - m_kBodyCopy2.m_kVelocity;
		temp.kRelAcceleration = m_kBodyCopy1.m_kAcceleration - m_kBodyCopy2.m_kAcceleration;
		temp.fAtime =	atime;
		temp.kCollissionTangent = (temp.kNormal.Cross(temp.kRelVelocity.Unit())).Cross(temp.kNormal);		
		
		m_kCollissions.push_back(temp);		
	}

	return false; // Zeblar: Vet inte vad den här funktionen skall returnera men jag chansar på FALSE :)

}

int Physics_Engine::CollideBody(Body* pkBody1,Body* pkBody2)
{
	float d = pkBody1->m_kPosition.DistanceTo(pkBody2->m_kPosition) - (pkBody1->m_fRadius + pkBody2->m_fRadius);
				
	if(fabs(d) <= m_fCtol)	
		return COLLISSION;
	
	if(d < 0)
		return PENETRATING;
				
	return NOT;	
}


void Physics_Engine::HandleCollission(Collission* pkCol)
{		

	//body VS body collission
	if(pkCol->pkBody2 != NULL)
	{
		//body awake from your slumber =)	
		pkCol->pkBody1->Awaken();
		pkCol->pkBody2->Awaken();
		
		float bounce = pkCol->pkBody1->m_fBounce*pkCol->pkBody2->m_fBounce;
		
		
		float j = (-(1+bounce) * (pkCol->kRelVelocity * pkCol->kNormal)) /
					 ( (pkCol->kNormal*pkCol->kNormal) *
					 ( 1/pkCol->pkBody1->m_fMass + 1/pkCol->pkBody2->m_fMass));
		
/*
		float j = -pkCol->kRelVelocity*(bounce+1) / 
					 ( (1/pkCol->pkBody1->m_fMass + 1/pkCol->pkBody2->m_fMass) +
					 (pkCol->kNormal * ((pkCol->kMassCenterToPos1.Cross(pkCol->kNormal)) / pkCol->pkBody1->m_kInertia).Cross(pkCol->kMassCenterToPos1)) +
 					 (pkCol->kNormal * ((pkCol->kMassCenterToPos2.Cross(pkCol->kNormal)) / pkCol->pkBody2->m_kInertia).Cross(pkCol->kMassCenterToPos2)));
*/
					 

		//if objects r almost resting towards each other give em a little extra push
		//cout<<"J:"<<j<<endl;
		if(j<0)
			j*=-1;
		
		//if objects r almost resting give it a little extra push		
		if(j <= 0.001)
		{	
			j=0.1;
		}

		//cout<<"BL:"<<j<<endl;

		float friction = 0.1;

		//apply impulse force
		pkCol->pkBody1->m_kVelocity += (j*pkCol->kNormal)  / pkCol->pkBody1->m_fMass;
		pkCol->pkBody1->m_kAngleVel += (pkCol->kMassCenterToPos1.Cross((j *pkCol->kNormal))) * pkCol->pkBody1->m_kInertiaInverse;  
		
		pkCol->pkBody2->m_kVelocity -= (j*pkCol->kNormal)  / pkCol->pkBody2->m_fMass;
		pkCol->pkBody2->m_kAngleVel += (pkCol->kMassCenterToPos2.Cross((j *pkCol->kNormal))) * pkCol->pkBody2->m_kInertiaInverse;  

/*		//apply impulse force
		pkCol->pkBody1->m_kVelocity += ((j*pkCol->kNormal) - ((friction*j) * pkCol->kCollissionTangent)) / pkCol->pkBody1->m_fMass;
		pkCol->pkBody1->m_kAngleVel += (pkCol->kMassCenterToPos1.Cross((j *pkCol->kNormal) + (( friction* j) *pkCol->kCollissionTangent))) * pkCol->pkBody1->m_kInertiaInverse;  
		
		pkCol->pkBody2->m_kVelocity -= ((j*pkCol->kNormal) - ((friction*j) * pkCol->kCollissionTangent)) / pkCol->pkBody2->m_fMass;
		pkCol->pkBody2->m_kAngleVel += (pkCol->kMassCenterToPos2.Cross((j *pkCol->kNormal) + (( friction* j) *pkCol->kCollissionTangent))) * pkCol->pkBody2->m_kInertiaInverse;  
*/
		//apply friction force
		pkCol->pkBody1->m_kVelocity -= (pkCol->kCollissionTangent * (j*friction)) / pkCol->pkBody1->m_fMass;
		pkCol->pkBody2->m_kVelocity += (pkCol->kCollissionTangent * (j*friction)) / pkCol->pkBody2->m_fMass;

/*		//apply impulse force
		pkCol->pkBody1->m_kVelocity += (j*pkCol->kNormal) / pkCol->pkBody1->m_fMass;
		pkCol->pkBody2->m_kVelocity -= (j*pkCol->kNormal) / pkCol->pkBody2->m_fMass;
*/

		//chevk if anybody wants to rest
		//UpdateResting(pkCol->pkBody1);				
		//UpdateResting(pkCol->pkBody2);				
	
	}	
	else //body vs plane collission
	{		
		float bounce = pkCol->pkBody1->m_fBounce;
		
		float j = (-(1+bounce) * (pkCol->kRelVelocity * pkCol->kNormal)) /
					 ( (pkCol->kNormal*pkCol->kNormal) *
					 ( 1/pkCol->pkBody1->m_fMass ));			
		
/*		float j = (-(1+bounce) * (pkCol->kRelVelocity * pkCol->kNormal)) /
					( (1/pkCol->pkBody1->m_fMass) + 
					(pkCol->kNormal * (((pkCol->kMassCenterToPos1.Cross(pkCol->kNormal)) *
					pkCol->pkBody1->m_kInertiaInverse).Cross(pkCol->kMassCenterToPos1))));

/*		float j = (-(1+bounce) * (pkCol->kRelVelocity * pkCol->kNormal)) / ( (1/pkCol->pkBody1->m_fMass) + 
					(pkCol->kNormal * ((pkCol->kMassCenterToPos1.Cross(pkCol->kNormal) * pkCol->pkBody1->m_kInertiaInverse).Cross(pkCol->kMassCenterToPos1))));
	*/	
		
		if(j<0)
			j*=-1;
		
		//if objects r almost resting give it a little extra push		
		if(j <= 0.001)
		{	
			j=0.1;
		}
		
		float friction = 0.1;
		
		//apply impulse force
		pkCol->pkBody1->m_kVelocity += (j*pkCol->kNormal)  / pkCol->pkBody1->m_fMass;		
		//pkCol->pkBody1->m_kVelocity += ((j*pkCol->kNormal) - ((friction*j) * pkCol->kCollissionTangent)) / pkCol->pkBody1->m_fMass;
		
//		Vector3 newvel = (pkCol->kMassCenterToPos1.Cross((j *pkCol->kNormal) + (( friction* j) *pkCol->kCollissionTangent))) * pkCol->pkBody1->m_kInertiaInverse;  		

		pkCol->pkBody1->m_kAngleVel += (pkCol->kMassCenterToPos1.Cross((j *pkCol->kNormal) + (( friction* j) *pkCol->kCollissionTangent))) * pkCol->pkBody1->m_kInertiaInverse;  
//		cout<<"bla:"<<newvel.x<<" "<<newvel.y<<" "<<newvel.z<<endl;
		
	
		//apply friction force
		pkCol->pkBody1->m_kVelocity -= (pkCol->kCollissionTangent * (j*friction)) / pkCol->pkBody1->m_fMass;

		//check if body can rest
		//UpdateResting(pkCol->pkBody1);	
	}
}



Collission* Physics_Engine::FindNextCollission()
{
	if(m_kCollissions.size() == 0)
		return NULL;
		
	float fClosest = 9999999999.0;
	Collission* pkClosest = NULL;
	
	for(unsigned int i=0;i<m_kCollissions.size();i++)
	{
		if(m_kCollissions[i].fAtime < fClosest)
		{
			fClosest = m_kCollissions[i].fAtime;
			pkClosest = &m_kCollissions[i];
		}
	}
	
	
	static Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));
	
		// Vim - is this the wrong way?
	// pkRender->Sphere(pkClosest->kPos,0.2,1,Vector3(1,0,0),true);
	
	
	return pkClosest;
}


void Physics_Engine::UpdateResting(Body* pkBody1)
{
	
	//check if bodys velocity is beyound resting treshold
	if(pkBody1->m_kVelocity.Length()<0.01)
		if(pkBody1->m_kAngleVel.Length()<0.1)
			pkBody1->Rest(NULL);	
}

bool Physics_Engine::BodyCollides(Body* pkBody)
{
	for(list<Body*>::iterator bodyit2 =  m_kBodys.begin(); bodyit2 != m_kBodys.end(); bodyit2++) 
	{
		if( pkBody == (*bodyit2)) 
			continue;
		
		if( pkBody->m_bResting && (*bodyit2)->m_bResting)
			continue;
		
		int check = CollideBody(&m_kBodyCopy1,&m_kBodyCopy2);	

		if(check == PENETRATING || check == COLLISSION)
		{
			return true;
		}
	}
	
	return false;
}

int Physics_Engine::CollideMeshVSPlane(Body* pkBody,Plane* pkPlane)
{
	if(pkBody->m_pkVertex == NULL)
		return NOT;
	
	bool clear=false;
	

	static Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));
		
	for(unsigned int i=0;i<pkBody->m_pkVertex->size();i++)	
	{
		Vector3 point = pkBody->TransRot((*pkBody->m_pkVertex)[i]);
		
		
//		pkRender->Sphere(point,0.1,1,Vector3(1,1,1),true);				
//		cout<<"HUBBA "<<point.x<<" "<< point.y<<" "<<point.z<<endl;

	
		if(!pkPlane->PointInside(point))
		{
			//clear temp collissions if a collission occurs
			if(!clear)
			{	
				m_kCollissionPoints.clear();
				clear=true;
			}
			
			//cout<<"BLUB:"<<endl;
			m_kCollissionPoints.push_back(i);
		}
	}
	
	if(clear == true)
	{	
		return PENETRATING;
	}
	else
		return NOT;
}



bool Physics_Engine::TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos,float fR)
{
	Plane side[6];
	
	
	Vector3 V1 = kVerts[1] - kVerts[0];
	Vector3 V2 = kVerts[2] - kVerts[1];	
	Vector3 V3 = kVerts[0] - kVerts[2];	
	
	side[0].m_kNormal = pkNormal->Cross(V1).Unit();
	side[1].m_kNormal = pkNormal->Cross(V2).Unit();	
	side[2].m_kNormal = pkNormal->Cross(V3).Unit();

	side[3].m_kNormal = (side[0].m_kNormal + side[2].m_kNormal).Unit();
	side[4].m_kNormal = (side[0].m_kNormal + side[1].m_kNormal).Unit();
	side[5].m_kNormal = (side[1].m_kNormal + side[2].m_kNormal).Unit();


	side[0].m_fD = -side[0].m_kNormal.Dot(kVerts[0]);
	side[1].m_fD = -side[1].m_kNormal.Dot(kVerts[1]);	
	side[2].m_fD = -side[2].m_kNormal.Dot(kVerts[2]);	

	side[3].m_fD = -side[3].m_kNormal.Dot(kVerts[0]);
	side[4].m_fD = -side[4].m_kNormal.Dot(kVerts[1]);	
	side[5].m_fD = -side[5].m_kNormal.Dot(kVerts[2]);	
	
	
	bool inside = true;
	
	for(int i=0;i<6;i++)
	{
		if(!side[i].SphereInside(kPos,fR)){
			inside=false;
		}
	}
	
	return inside;	
}



