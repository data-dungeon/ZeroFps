#include "physics_engine.h"

Physics_Engine::Physics_Engine() 
: ZFObject("Physics_Engine")
{
	m_fCtol =		0.01;
	m_fTtol =		0.0001;
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
	int iNrOfCollissions=0;

	UpdateForces();	
	while(m_fRTime > 0)
	{
		TestWithPlanes(m_fRTime);
		TestBodys(m_fRTime);
		
		if(m_kCollissions.size() > 0)
		{
			iNrOfCollissions++;	
		
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

	//cout<<"TOTAL NR OF COLLISSIONS: "<<iNrOfCollissions<<endl;
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
	
		if(!(*it)->m_bResting)
		{
			if((*it)->m_bGravity)
				(*it)->m_kForces += Vector3(0,-8.82,0);
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

void Physics_Engine::TestWithPlanes(float fATime)
{
	for(list<Plane*>::iterator planeit = m_kPlanes.begin(); planeit != m_kPlanes.end(); planeit++) 
	{
		for(list<Body*>::iterator bodyit = m_kBodys.begin(); bodyit != m_kBodys.end(); bodyit++) 
		{
			if(!(*bodyit)->m_bResting)
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
	Body BodyCopy;
	bool retry = true;
	bool didpen = false;
	bool collission = false;
	int nroftests = 0;
	
	Collission tempcol;
	
	while(retry && nroftests < m_iMaxTests)
	{
		retry = false;
		nroftests++;
		
		memcpy(&BodyCopy,pkBody,sizeof(Body));
		
		UpdateBodyVelNPos(&BodyCopy,atime);
	
		int check = CollideSphereVSPlane(&BodyCopy,pkPlane);			
		
		if(check == PENETRATING || check == COLLISSION)
		{
			//is it the first penetration in test?
			if(didpen != true)
			{
				memcpy(&BodyCopy,pkBody,sizeof(Body));		
				int check2 = CollideSphereVSPlane(&BodyCopy,pkPlane);			
			
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
			atime /=2;//1.3;//-= 0.001;//(fATime/20);
			
			continue;
		}
		
		if(check == NOT && didpen)
		{
			break;
		}
		
		if(check == NOT && didpen == false)
			return false;
		
		if(atime < 0.0001)
		{	
			atime =0;
			break;
		}	

	}
	
		
	if(didpen)
	{	
		tempcol.pkBody1 = pkBody;
		tempcol.pkBody2 = NULL;
		
		tempcol.kPos = BodyCopy.m_kPosition - (pkPlane->m_kNormal * BodyCopy.m_fRadius);
		tempcol.kNormal = pkPlane->m_kNormal;	
		tempcol.kRelVelocity = BodyCopy.m_kVelocity;
		tempcol.kRelAcceleration = BodyCopy.m_kAcceleration;
		tempcol.fAtime =	atime;	
		
		//har ingen jävla aning om vad denna skall sättas till för tillfället =(
		tempcol.kCollissionTangent = Vector3(0,0,0);	
	
		m_kCollissions.push_back(tempcol);	
	
		return true;
	}
	
	return false;
}

void Physics_Engine::UpdateBodyVelNPos(Body* pkBody,float fAtime)
{
		Vector3 kAe;
		
		//Calculate acceleration in world space
		kAe = pkBody->m_kForces / pkBody->m_fMass;
		pkBody->m_kAcceleration = kAe;
		
		//Calculate velocity in world space
		pkBody->m_kVelocity += kAe * fAtime;
		
		//Calculate velocity in world space
		pkBody->m_kPosition += pkBody->m_kVelocity * fAtime;
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
	Body BodyCopy1;
	Body BodyCopy2;
		
	bool retry = true;
	bool didpen = false;
	bool collission = false;
	
	int nroftests = 0;
	
	while(retry && nroftests < m_iMaxTests)
	{
		retry = false;
		nroftests++;
		
		memcpy(&BodyCopy1,pkBody1,sizeof(Body));
		memcpy(&BodyCopy2,pkBody2,sizeof(Body));
		
		UpdateBodyVelNPos(&BodyCopy1,atime);
		UpdateBodyVelNPos(&BodyCopy2,atime);	
	
		int check = CollideBody(&BodyCopy1,&BodyCopy2);			
		
		
		if(check == PENETRATING || check == COLLISSION)
		{
			//is it the first penetration in test?
			if(didpen != true)
			{
				memcpy(&BodyCopy1,pkBody1,sizeof(Body));
				memcpy(&BodyCopy2,pkBody2,sizeof(Body));
				
				int check2 = CollideBody(&BodyCopy1,&BodyCopy2);			
			
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
			atime /=2;//1.3;//-= 0.001;//(fATime/20);
		
			continue;
		}
		
		if(check == NOT && didpen)
		{
			break;
		}
		
		if(check == NOT && didpen == false)
			return false;
		

		if(atime < 0.0001)
		{	
			atime =0;
			break;
		}	

	}	
	
	if(didpen)
	{	
		Vector3 relv = (BodyCopy1.m_kVelocity - BodyCopy2.m_kVelocity);
		Vector3 norm = (BodyCopy1.m_kPosition - BodyCopy2.m_kPosition).Unit();
	
		//only collide if relative velocity is towards each other
		//cout<<norm.Dot(relv)<<endl;
		
//		if( norm.Dot(relv) < 0)
//		{
	
			Collission temp;
			temp.pkBody1 = pkBody1;
			temp.pkBody2 = pkBody2;
		
			temp.kNormal = norm;
			temp.kPos = BodyCopy1.m_kPosition - (temp.kNormal * BodyCopy1.m_fRadius);	
			temp.kRelVelocity = relv;
			temp.kRelAcceleration = BodyCopy1.m_kAcceleration - BodyCopy2.m_kAcceleration;
			temp.fAtime =	atime;

		
			//har ingen jävla aning om vad denna skall sättas till för tillfället =(
			temp.kCollissionTangent = Vector3(0,0,0);
		
			m_kCollissions.push_back(temp);		
//		}
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
		pkCol->pkBody1->m_bResting = false;
		pkCol->pkBody2->m_bResting = false;		
		
		float bounce = pkCol->pkBody1->m_fBounce*pkCol->pkBody2->m_fBounce;
		float j = (-(1+bounce) * (pkCol->kRelVelocity * pkCol->kNormal)) /
					 ( (pkCol->kNormal*pkCol->kNormal) *
					 ( 1/pkCol->pkBody1->m_fMass + 1/pkCol->pkBody2->m_fMass));

	
		pkCol->pkBody1->m_kVelocity += (j*pkCol->kNormal) / pkCol->pkBody1->m_fMass;
		pkCol->pkBody2->m_kVelocity -= (j*pkCol->kNormal) / pkCol->pkBody2->m_fMass;

//		UpdateResting(pkCol->pkBody1,pkCol->pkBody2);	
	}	
	else //body vs plane collission
	{		
		float bounce = pkCol->pkBody1->m_fBounce;
		float j = (-(1+bounce) * (pkCol->kRelVelocity * pkCol->kNormal)) /
					 ( (pkCol->kNormal*pkCol->kNormal) *
					 ( 1/pkCol->pkBody1->m_fMass + 1/9999999999));			
		
		pkCol->pkBody1->m_kVelocity += (j*pkCol->kNormal) / pkCol->pkBody1->m_fMass;

//		UpdateResting(pkCol->pkBody1,NULL);
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


void Physics_Engine::UpdateResting(Body* pkBody1,Body* pkBody2)
{
	if(pkBody1->m_kVelocity.Length()<0.01)
	{
		pkBody1->m_bResting=true;
		pkBody1->m_kVelocity.Set(0,0,0);
	
		cout<<"SLEEEP"<<endl;
	}
	
	if(pkBody2 != NULL)
	{
		if(pkBody2->m_kVelocity.Length()<0.01)
		{	
			pkBody2->m_bResting=true;
			pkBody2->m_kVelocity.Set(0,0,0);
		
			cout<<"SLEEP"<<endl;
		}	
	}



}











