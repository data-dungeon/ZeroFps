#include "physics_engine.h"

Physics_Engine::Physics_Engine() 
: ZFObject("Physics_Engine")
{
	m_fCtol =		0.01;
	m_iMaxTests =	20;
	
	Clear();
}

void Physics_Engine::Clear()
{
	m_kBodys.clear();
}

void Physics_Engine::Update(float fAlphaTime)
{	
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
			if(pkCol->fAtime < 0.0001)
				pkCol->fAtime = 0.0001;
					
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
				(*it)->m_kForces += Vector3(0,-9.82,0);
				
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
		
		//Calculate velocity in world space
		pkBody->m_kPosition += pkBody->m_kVelocity * fAtime;
		
		
		//rotation
				
		//calculate angular acceleration
		pkBody->m_kAngleAcceleration = pkBody->m_kInertiaInverse *
												(pkBody->m_kMoment -
												(pkBody->m_kAngleVel.Cross(pkBody->m_kInertia*pkBody->m_kAngleVel)));
	
		//calculate new rotation velocity
		pkBody->m_kAngleVel += pkBody->m_kAngleAcceleration * fAtime;
		
		//calculate new orientation
		pkBody->m_kOrientation += (pkBody->m_kOrientation * pkBody->m_kAngleVel)* (0.5 * fAtime);
		
		float mag = pkBody->m_kOrientation.Length();
		if(mag !=0)
			pkBody->m_kOrientation /= mag;
			
		//calculate velocity in body cordinats
		pkBody->m_kBodyVelocity = pkBody->m_kOrientation.RotateVector3(pkBody->m_kVelocity);
		
		//calculate body speed
		pkBody->m_fSpeed = pkBody->m_kVelocity.Length();
		
		//get euler angles
		pkBody->m_kOrientation.ToAxes(&pkBody->m_kAngles);

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
	
		int check = CollideMeshVSPlane(&m_kBodyCopy1,pkPlane);			
		
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
		
		//if time is infinit short set it to 0	
		if(atime < 0.0001)
		{	
			atime =0;
			break;
		}	

	}
	
		
	if(didpen)
	{	
		//add all temp collissions
		for(int i=0;i<m_kCollissionPoints.size();i++)
		{
			Collission tempcol;

			tempcol.pkBody1 = pkBody;
			tempcol.pkBody2 = NULL;
		
//			Vector3 p;// =m_kBodyCopy1.TransRot(m_kBodyCopy1.m_kVertex[m_kCollissionPoints[i]]);
//			p=m_kBodyCopy1.m_kPosition - (pkPlane->m_kNormal * m_kBodyCopy1.m_fRadius) + Vector3(0.1,0,0);
//			cout<<p.x<<" "<<p.y<<" "<<p.z<<endl;
			
			tempcol.kPos = m_kBodyCopy1.TransRot(m_kBodyCopy1.m_kVertex[m_kCollissionPoints[i]]);
			tempcol.kMassCenterToPos1 = (tempcol.pkBody1->m_kPosition + tempcol.pkBody1->m_kMassCenter) - tempcol.kPos;
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
			
		//if time is infinit short set it to 0	
		if(atime < 0.0001)
		{	
			atime =0;
			break;
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

}

int Physics_Engine::CollideBody(Body* pkBody1,Body* pkBody2)
{
	float d = (pkBody1->m_kPosition - pkBody2->m_kPosition).Length() - (pkBody1->m_fRadius + pkBody2->m_fRadius);
				
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
		if(j <= 0.0005)
		{	
			j+=0.2;
		}

		//apply impulse force
		pkCol->pkBody1->m_kVelocity += (j*pkCol->kNormal) / pkCol->pkBody1->m_fMass;
		pkCol->pkBody2->m_kVelocity -= (j*pkCol->kNormal) / pkCol->pkBody2->m_fMass;

		//apply friction force
		pkCol->pkBody1->m_kVelocity -= (pkCol->kCollissionTangent * (j*0.2)) / pkCol->pkBody1->m_fMass;
		pkCol->pkBody2->m_kVelocity += (pkCol->kCollissionTangent * (j*0.2)) / pkCol->pkBody2->m_fMass;

		//chevk if anybody wants to rest
		UpdateResting(pkCol->pkBody1);				
		UpdateResting(pkCol->pkBody2);				
	
	}	
	else //body vs plane collission
	{		
		float bounce = pkCol->pkBody1->m_fBounce;
		
/*		float j = (-(1+bounce) * (pkCol->kRelVelocity * pkCol->kNormal)) /
					 ( (pkCol->kNormal*pkCol->kNormal) *
					 ( 1/pkCol->pkBody1->m_fMass + 1/9999999999));			
*/		
		Vector3 ptr1= pkCol->kPos - pkCol->pkBody1->m_kPosition;

		float j = (-(1+bounce) * (pkCol->kRelVelocity * pkCol->kNormal)) /
					( (1/pkCol->pkBody1->m_fMass) + 
					(pkCol->kNormal * (((pkCol->kMassCenterToPos1.Cross(pkCol->kNormal)) *
					pkCol->pkBody1->m_kInertiaInverse).Cross(pkCol->kMassCenterToPos1))));

		//if objects r almost resting give it a little extra push		
		if(j <= 0.0005)
		{	
			j+=0.2;
		}
		
		float friction = 0.1;
		
		//apply impulse force
		pkCol->pkBody1->m_kVelocity += ((j*pkCol->kNormal) - ((friction*j) * pkCol->kCollissionTangent)) / pkCol->pkBody1->m_fMass;
		pkCol->pkBody1->m_kAngleVel += (pkCol->kMassCenterToPos1.Cross((j *pkCol->kNormal) + (( friction* j) *pkCol->kCollissionTangent))) * pkCol->pkBody1->m_kInertiaInverse;  
	
//		//apply friction force
//		pkCol->pkBody1->m_kVelocity -= (pkCol->kCollissionTangent * (j*0.1)) / pkCol->pkBody1->m_fMass;

		//check if body can rest
		UpdateResting(pkCol->pkBody1);	
	}
}



Collission* Physics_Engine::FindNextCollission()
{
	if(m_kCollissions.size() == 0)
		return NULL;
		
	float fClosest = 9999999999;
	Collission* pkClosest = NULL;
	
	for(int i=0;i<m_kCollissions.size();i++)
	{
		if(m_kCollissions[i].fAtime < fClosest)
		{
			fClosest = m_kCollissions[i].fAtime;
			pkClosest = &m_kCollissions[i];
		}
	}
	
	return pkClosest;
}


void Physics_Engine::UpdateResting(Body* pkBody1)
{
	
	//check if bodys velocity is beyound resting treshold
	if(pkBody1->m_kVelocity.Length()<0.01)
	{
		pkBody1->Rest(NULL);	
	}
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
	
		
	for(int i=0;i<pkBody->m_pkVertex->size();i++)	
	{
		Vector3 point = pkBody->TransRot((*pkBody->m_pkVertex)[i]);
		
	
		if(!pkPlane->PointInside(point))
		{
			//clear temp collissions if a collission occurs
			if(!clear)
			{	
				m_kCollissionPoints.clear();
				clear=true;
			}
			
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








