#include "tcs.h"
#include "../propertys/p_tcs.h"
#include "../common/heightmap.h"
	
Tcs::Tcs(): ZFSubSystem("Tcs")
{
	Logf("zerofps","Tiny Collission system created");

 	m_fMaxDelay = 		0.04;
	m_fAlmostZero = 	0.001;
	m_fMinForce = 		0.2;	
	m_fSleepVel = 		0.3;
	m_fMaxVel = 		10.0;
	m_fTimeSlice = 	2.0;
	
	m_iHandleCollission = 1;
	m_iDebugGraph = 0;
	
	m_iNrOfCollissions = 0;
	m_iNrOfTests = 0;
	
	RegisterVariable("p_tcshandle",	&m_iHandleCollission,CSYS_INT);
	RegisterVariable("p_tcsdebug",	&m_iDebugGraph,CSYS_INT);
	RegisterVariable("p_tcsdelay",	&m_fMaxDelay,CSYS_FLOAT);
	RegisterVariable("p_tcsminforce",&m_fMinForce,CSYS_FLOAT);
	RegisterVariable("p_tcssleepvel",&m_fSleepVel,CSYS_FLOAT);
	RegisterVariable("p_tcsmaxvel",	&m_fMaxVel,CSYS_FLOAT);
}

Tcs::~Tcs()
{
	delete m_pkBodyCopy1;
	delete m_pkBodyCopy2;
}


bool Tcs::StartUp()	
{ 
	m_pkZeroFps		= static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));		
	m_pkRender		= static_cast<Render*>(GetSystem().GetObjectPtr("Render"));		

	m_pkBodyCopy1 = new P_Tcs();
	m_pkBodyCopy2 = new P_Tcs();	
		
	return true; 
}

bool Tcs::ShutDown() 
{ 
	return true; 
}
bool Tcs::IsValid()	
{ 
	return true; 
}



void Tcs::AddBody(P_Tcs* pkPTcs)
{
 	//check so its not added twice
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{
		if(m_kBodys[i] == pkPTcs)
			return;
	
	}
	
	m_kBodys.push_back(pkPTcs);
}
void Tcs::RemoveBody(P_Tcs* pkPTcs)
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{
		if(m_kBodys[i] == pkPTcs)
		{	
			m_kBodys[i]->Wakeup();
		
			m_kBodys[i]=m_kBodys.back();			//chacke place whit bak elment 
			m_kBodys.pop_back();
			
			return;
		}
	}
}



void Tcs::Update(float fAlphaTime)
{
	if(m_kBodys.empty())
		return;
	
		
	float fStartTime 		= m_pkZeroFps->GetTicks();
	float fRTime 			= fAlphaTime;
	m_iNrOfCollissions	= 0;
	m_iNrOfTests			= 0;
	m_iNrOfActiveBodies	= 0;	
	
	//synd all bodys to entitys
	SyncBodys();
	
	//make sure there is no collissions	
	m_kLastCollission.Clear();
	
	//calculate current forces
	UpdateForces();
	
	
	while(fRTime > 0)
	{
		//do collissiontests
		UpdateCollissions(fRTime);
	
		//did any collission ocur?
		if(m_kLastCollission.fAtime != -1)		
		{
			m_iNrOfCollissions++;
		
			//update all objects up to current time
			UpdateAllVelnPos(m_kLastCollission.fAtime);			
			
			//handle collission
			HandleCollission(&m_kLastCollission);
				
			//set new Resttime			
			fRTime -= m_kLastCollission.fAtime;			
						
			//clear all collissions
			m_kLastCollission.Clear();

			// make sure we dont spend to much time doing physics =)
			if((m_pkZeroFps->GetTicks() - fStartTime) > m_fMaxDelay)
				break;								
		}
		//no collissions, just update time and continue
		else 
		{
			UpdateAllVelnPos(fRTime);	
			fRTime = 0;
			break;
		}	
	}
	
	//update all character ground line tests
	UpdateLineTests(fAlphaTime);
	
	//synd all entitys to bodys
	SyncEntitys();
	
	//clear all forces
	ResetForces();
	
}

void Tcs::UpdateLineTests(float fAlphaTime)
{
	float distance;

	for(unsigned int i=0;i<m_kBodys.size();i++)
	{				
		if(m_kBodys[i]->m_bCharacter)
		{	
			if(TestLine(m_kBodys[i]->m_kNewPos,Vector3(0,-1,0),m_kBodys[i]))
			{
				distance = m_kBodys[i]->m_kNewPos.DistanceTo(m_kLastTestPos);
				if(distance < m_kBodys[i]->m_fLegLength)
				{
					m_kBodys[i]->m_kNewPos = m_kLastTestPos + Vector3(0,m_kBodys[i]->m_fLegLength,0);
					m_kBodys[i]->m_bOnGround = true;
					m_kBodys[i]->m_kLinearVelocity.y= 0;
				
					//if(m_kBodys[i]->m_kWalkVel.Length() == 0)
					//	m_kBodys[i]->m_kLinearVelocity.Set(0,0,0);
				}			
				else
					m_kBodys[i]->m_bOnGround = false;			
					

				//add breaks to character :D					
				float fOld = m_kBodys[i]->m_kLinearVelocity.y;
				m_kBodys[i]->m_kLinearVelocity -= 10*(m_kBodys[i]->m_kLinearVelocity * fAlphaTime);						
				
				//m_kBodys[i]->m_kLinearVelocity.Set(0,fOld,0);
				m_kBodys[i]->m_kLinearVelocity.y = fOld;
				
			}
		}
	}
}

void Tcs::HandleCollission(Tcs_collission* pkCol)
{	
	//wake up bodys
	pkCol->pkBody1->Wakeup();
	pkCol->pkBody2->Wakeup();

	//nr of collision points
	int iNrOfPos = pkCol->kPositions.size();
	
	// masses
	float fMass1 = pkCol->pkBody1->m_fMass;
	float fMass2 = pkCol->pkBody2->m_fMass;
			
	//bounce
	float fBounce = pkCol->pkBody1->m_fBounce * pkCol->pkBody2->m_fBounce;
	
	//friction
	float fFriction = pkCol->pkBody1->m_fFriction * pkCol->pkBody2->m_fFriction;
	
	//setup vels	
	Vector3 kLinearRelVel = pkCol->pkBody1->m_kLinearVelocity - pkCol->pkBody2->m_kLinearVelocity;
	Vector3 kRotVel1 = pkCol->pkBody1->m_kRotVelocity;
	Vector3 kRotVel2 = pkCol->pkBody2->m_kRotVelocity;

	//if thers no relativ velocity, ther can be no collission
	//if(kLinearRelVel.Length() == 0)
	//	return;	
	
	//setup Masses, treat static bodys as having infinit mass	
	if(pkCol->pkBody1->m_bStatic)
	{
		fMass1 = 999999999;	
	}
	
	if(pkCol->pkBody2->m_bStatic)
	{
		fMass2 = 999999999;	
	}
	
//	cout<<"v1:"<<kRotVel1.Length()<<" "<<pkCol->pkBody1->m_kLinearVelocity.Length()<<endl;
//	cout<<"v2:"<<kRotVel2.Length()<<" "<<pkCol->pkBody2->m_kLinearVelocity.Length()<<endl;
		
	float fTotalj = 0;
	
	for(int i = 0;i<	iNrOfPos;i++)
	{	
		if(m_iDebugGraph)
		{		
			//m_pkRender->Sphere(pkCol->kPositions[i],0.2,1,Vector3(0,1,0),false);			
			m_pkRender->SetColor(Vector3(1,0,1));
			m_pkRender->Line(pkCol->kPositions[i],pkCol->kPositions[i] + pkCol->kNormals[i]);		
			m_pkRender->SetColor(Vector3(1,1,1));		
		}
		
		//collission handling is disabled
		if(!m_iHandleCollission)
			continue;

		//calculate the relative velocity betwen collission points
		Vector3 kRelVel = kLinearRelVel + (-kRotVel1.Cross(pkCol->kPositions[i] - pkCol->pkBody1->m_kNewPos) - -kRotVel2.Cross(pkCol->kPositions[i] - pkCol->pkBody2->m_kNewPos));		
	
		if(kRelVel.Length() == 0)
		{
			//this shuld not hapen...but it does...=(
			//TryToSleep(pkCol->pkBody1,pkCol->pkBody2);		
			return;			
		}
		
		//calculate collission normal
		Vector3 kTangent = (pkCol->kNormals[i].Cross(kRelVel.Unit())).Cross(pkCol->kNormals[i]);
					
		//calculcate collision impact force				
		float j  = (-(1+fBounce) * (kRelVel * pkCol->kNormals[i])) /
					  ( (pkCol->kNormals[i] * pkCol->kNormals[i]) *
					  ( 1/fMass1 + 1/fMass2)); 	
		
		//make sure the impact force is not to small					  
		if(j < m_fMinForce)
			j = m_fMinForce;  
		
		//divide force whit nr of impact points
		j /= float(iNrOfPos);
					
  		fTotalj+=j;
								  
		// APPLY IMPULSES					
		if(!pkCol->pkBody1->m_bStatic)
		{
			//pkCol->pkBody1->ApplyImpulsForce(pkCol->kNormals[i] * j);
			//pkCol->pkBody1->ApplyImpulsForce((-kTangent * j)*fFriction);
			pkCol->pkBody1->ApplyImpulsForce(pkCol->kPositions[i],pkCol->kNormals[i] * j ,false);							
			pkCol->pkBody1->ApplyImpulsForce(pkCol->kPositions[i],-kTangent * (j * fFriction) ,false);
		}		
		
		if(!pkCol->pkBody2->m_bStatic)
		{
			//pkCol->pkBody2->ApplyImpulsForce(-pkCol->kNormals[i] * j);
			//pkCol->pkBody2->ApplyImpulsForce((kTangent * j)*fFriction);			
			pkCol->pkBody2->ApplyImpulsForce(pkCol->kPositions[i],-pkCol->kNormals[i] * j ,false);							
			pkCol->pkBody2->ApplyImpulsForce(pkCol->kPositions[i],kTangent * (j * fFriction) ,false);
		}					  
	}
	
	//touch objects
	pkCol->pkBody1->GetObject()->Touch(pkCol->pkBody2->GetObject()->GetEntityID());
	pkCol->pkBody2->GetObject()->Touch(pkCol->pkBody1->GetObject()->GetEntityID());	

	//try to put bodys to sleep
	TryToSleep(pkCol->pkBody1,pkCol->pkBody2);						
}

void Tcs::SyncEntitys()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
		if(m_iDebugGraph == 3)		
			if(!m_kBodys[i]->m_bSleeping && !m_kBodys[i]->m_bStatic)
				m_pkRender->Sphere(m_kBodys[i]->m_kNewPos,m_kBodys[i]->m_fRadius ,1,Vector3(1,0,0),false);
			else
				m_pkRender->Sphere(m_kBodys[i]->m_kNewPos,m_kBodys[i]->m_fRadius ,1,Vector3(0,1,0),false);
					
			
		m_kBodys[i]->GetObject()->SetWorldPosV(m_kBodys[i]->m_kNewPos);
		m_kBodys[i]->GetObject()->SetVel(m_kBodys[i]->m_kLinearVelocity);
		m_kBodys[i]->GetObject()->SetLocalRotM(m_kBodys[i]->m_kNewRotation);
	}
}

void Tcs::SyncBodys()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{			
		m_kBodys[i]->m_kNewPos = m_kBodys[i]->GetObject()->GetWorldPosV();		
		m_kBodys[i]->m_kNewRotation = m_kBodys[i]->GetObject()->GetLocalRotM();
		//m_kBodys[i]->m_kLinearVelocity = m_kBodys[i]->GetObject()->GetVel();							
	}
}

void Tcs::UpdateForces()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
		if(m_kBodys[i]->m_bStatic || m_kBodys[i]->m_bSleeping)
			continue;

		//increse active body count
		m_iNrOfActiveBodies++;
			
				
		// LINEAR FORCE / ACCLERERATION			
			m_kBodys[i]->m_kLinearForce.Set(0,0,0);
		
			//add external linear force
			m_kBodys[i]->m_kLinearForce += m_kBodys[i]->m_kExternalLinearForce; 
					
			//apply gravity if enabled
			if(m_kBodys[i]->m_bGravity)
			{
				m_kBodys[i]->m_kLinearForce.y -= 9.00 * m_kBodys[i]->m_fMass;
			}
			
			//apply some air friction		
			m_kBodys[i]->m_kLinearForce -= m_kBodys[i]->m_kLinearVelocity * m_kBodys[i]->m_fAirFriction;
	
			//Calculate linear acceleration
			m_kBodys[i]->m_kLinearForce = m_kBodys[i]->m_kLinearForce / m_kBodys[i]->m_fMass;	
				
		
		
		// ROTATIONAL FORCE / ACCLERATION			
			m_kBodys[i]->m_kRotForce.Set(0,0,0);
			
			//add external rotation force
			m_kBodys[i]->m_kRotForce += m_kBodys[i]->m_kExternalRotForce; 		
			
			//apply some air friction				
			m_kBodys[i]->m_kRotForce -= m_kBodys[i]->m_kRotVelocity * (m_kBodys[i]->m_fAirFriction*2);
	
			//calculate rotation acceleration
			m_kBodys[i]->m_kRotForce = m_kBodys[i]->m_kRotForce * m_kBodys[i]->m_fInertia;
		
	}
}

void Tcs::UpdateAllVelnPos(float fAtime)
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
			
		UpdateBodyVelnPos(m_kBodys[i],fAtime);				
	}
}



void Tcs::UpdateBodyVelnPos(P_Tcs* pkBody,float fAtime)
{
	if(pkBody->m_bStatic || pkBody->m_bSleeping)
		return;
	
		
				
	//apply linear acceleration
	pkBody->m_kLinearVelocity += pkBody->m_kLinearForce * fAtime;	
	//apply rotation acceleration
	pkBody->m_kRotVelocity += pkBody->m_kRotForce * fAtime;

	
	//check max velocitys
	if(pkBody->m_kLinearVelocity.Length() > m_fMaxVel)
		pkBody->m_kLinearVelocity = pkBody->m_kLinearVelocity.Unit() * m_fMaxVel;
	
		
	
	//Calculate new position
	pkBody->m_kNewPos += (pkBody->m_kLinearVelocity * fAtime);
	//calculate new rotation 
	pkBody->m_kNewRotation.RadRotate(pkBody->m_kRotVelocity * fAtime);

}

void Tcs::UpdateCollissions(float fAtime)
{
	for(unsigned int B1=0;B1<m_kBodys.size();B1++)
	{
		for(unsigned int B2=B1+1;B2<m_kBodys.size();B2++)
		{
			//dont test if both bodys are static or sleeping
			if( (m_kBodys[B1]->m_bStatic || m_kBodys[B1]->m_bSleeping) && (m_kBodys[B2]->m_bStatic || m_kBodys[B2]->m_bSleeping))
				continue;		
				
			if(m_kBodys[B1]->m_akTestGroups[m_kBodys[B2]->m_iGroup] ||
				m_kBodys[B2]->m_akTestGroups[m_kBodys[B1]->m_iGroup])			
			{			
				//first do a motion sphere test
				if(TestMotionSpheres(m_kBodys[B1],m_kBodys[B2],fAtime))
				{										
					if(m_kBodys[B1]->m_bPolygonTest && m_kBodys[B2]->m_bPolygonTest)
					{
						//MESH VS MESH
						TestMeshVsMesh(m_kBodys[B1],m_kBodys[B2],fAtime,&m_kLastCollission);
					}
					else if(m_kBodys[B1]->m_bPolygonTest || m_kBodys[B2]->m_bPolygonTest)
					{				
						//SPHERE VS SPHERE
						TestSphereVsMesh(m_kBodys[B1],m_kBodys[B2],fAtime,&m_kLastCollission);
					}
					else
					{
						//SPHERE VS SPHERE
						TestSphereVsSphere(m_kBodys[B1],m_kBodys[B2],fAtime,&m_kLastCollission);
					}				
				}
			}
		}
	}
}


void Tcs::HandleCharacterCollission(P_Tcs* pkCharacter,P_Tcs* pkBody)
{
	if(pkCharacter->m_akWalkableGroups[pkBody->m_iGroup])
	{
			
		Vector3 dir = (m_kLastTestPos - pkCharacter->GetObject()->GetWorldPosV()).Unit();
		
		pkCharacter->m_kNewPos = pkCharacter->GetObject()->GetWorldPosV() - dir * 0.1;
	}
}

bool Tcs::TestMotionSpheres(P_Tcs* pkBody1,P_Tcs* pkBody2,const float& fAtime)
{
	static Vector3 kMedPos1;
	static Vector3 kMedPos2;
	static float d;	
	
	memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
	memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));		
	UpdateBodyVelnPos(m_pkBodyCopy1,fAtime);
	UpdateBodyVelnPos(m_pkBodyCopy2,fAtime);	

	kMedPos1 = (pkBody1->m_kNewPos + m_pkBodyCopy1->m_kNewPos) / 2.0;
	kMedPos2 = (pkBody2->m_kNewPos + m_pkBodyCopy2->m_kNewPos) / 2.0;

	d = kMedPos1.DistanceTo(kMedPos2) - (pkBody1->m_fRadius + pkBody2->m_fRadius);

	return (d <= 0);
}


bool Tcs::TestSides(Vector3* kVerts,Vector3* pkNormal,const Vector3& kPos)
{
/*  
  // first way to do it :D
  Vector3 vert0p = kVerts[0] - kPos;
  Vector3 vert1p = kVerts[1] - kPos;
  float d = vert0p.Cross(vert1p).Dot(*pkNormal);
  if (d < 0) return false;
  Vector3 vert2p = kVerts[2] - kPos;
  d = vert1p.Cross(vert2p).Dot(*pkNormal);
  if (d < 0) return false;
  d = vert2p.Cross(vert0p).Dot(*pkNormal);
  if (d < 0) return false;
  return true;
*/


	//one way to do it
  Vector3 e10=kVerts[1]-kVerts[0];
  Vector3 e20=kVerts[2]-kVerts[0];
  float a = e10.Dot(e10);
  float b = e10.Dot(e20);
  float c = e20.Dot(e20);
  float ac_bb=(a*c)-(b*b);
  Vector3 vp(kPos.x-kVerts[0].x, kPos.y-kVerts[0].y, kPos.z-kVerts[0].z);
  float d = vp.Dot(e10);
  float e = vp.Dot(e20);
  float x = (d*c)-(e*b);
  float y = (e*a)-(d*b);
  float z = x+y-ac_bb;
  return (( ((unsigned int &)z) & ~(((unsigned int &)x)|((unsigned int &)y)) ) & 0x80000000) != 0;

	

/*	
	//3 way do do it :P
	static Plane side[6];
	static Vector3 V1,V2,V3;
	
	
	V1 = kVerts[1] - kVerts[0];
	V2 = kVerts[2] - kVerts[1];	
	V3 = kVerts[0] - kVerts[2];	
	
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
	
	
	for(int i=0;i<6;i++)
	{
		if(!side[i].PointInside(kPos))
			return false;
	}
	 
	return true;
	*/
	
}


P_Tcs* Tcs::TestLine(Vector3 kStart,Vector3 kDir,P_Tcs* pkTester)
{
	
	
	m_kLastTestPos = kStart;		
	float closest = 999999999;
	P_Tcs* pkClosest = NULL;	
	float d;
	
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{		
		if(m_kBodys[i] == pkTester)
			continue;
			
		if(!pkTester->m_akWalkableGroups[m_kBodys[i]->m_iGroup])
			continue;			
		
		
		if(TestLineVSMesh(kStart,kDir,m_kBodys[i]))
		{
			d = kStart.DistanceTo(m_kLastLineTestColPos);				
			if(d < closest)
			{
				
				m_kLastTestPos = m_kLastLineTestColPos;
				closest = d;
				pkClosest = m_kBodys[i];
			}				
		}		
	}		
	
	return pkClosest;
}

bool Tcs::TestLineVSMesh(Vector3 kStart,Vector3 kDir,P_Tcs* pkB)
{
	Vector3 kColPos;

	if(pkB->m_pkMad)
	{	
		if(pkB->m_pkMad->TestLine(kStart,kDir))
		{
			m_kLastLineTestColPos = pkB->m_pkMad->GetLastColPos();
			return true;
		}
	}
	else if (pkB->m_pkHmap) {
			if(pkB->LineVSMesh(kStart,kDir))
			{
				m_kLastLineTestColPos = pkB->m_kColPos;
				return true;
			}
		}



	return false;	
}

bool Tcs::TestLineVSSphere(Vector3 kP1,Vector3 kP2,P_Tcs* pkB)
{
	Vector3 kDir = kP2 - kP1;

	Vector3 c=pkB->m_kNewPos - kP1;		

	float d = kDir.Unit().Dot(c);
	if(d < 0)
		return false;
	if(d > kDir.Length())
		return false;
	
	kDir.Normalize();		
	Vector3 k=kDir.Proj(c);		
	float cdis=c.Length();
	float kdis=k.Length();
	float Distance = (float) sqrt((cdis*cdis)-(kdis*kdis));
	

	if(Distance < pkB->m_fRadius)
	{			
		m_kLastTestPos = kP1 + k;
		return true;
	}		
	
	return false;
}


void Tcs::TestSphereVsSphere(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime,Tcs_collission* pkCollission)
{
	bool retry = true;
	bool didpen = false;
	int nroftests = 0;
	
	while(retry && (fAtime > 0 ))
	{
		retry = false;
		nroftests++;
		
		if(fAtime < m_fAlmostZero)
			fAtime = 0;
		
		memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
		memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));		
		UpdateBodyVelnPos(m_pkBodyCopy1,fAtime);
		UpdateBodyVelnPos(m_pkBodyCopy2,fAtime);	
	
		if(CollideSphereVSSphere(m_pkBodyCopy1,m_pkBodyCopy2))
		{
			//if first penetration do a check at time 0
			if(!didpen)
			{
				memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
				memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));		
			
				UpdateBodyVelnPos(m_pkBodyCopy1,0);
				UpdateBodyVelnPos(m_pkBodyCopy2,0);				
				
				if(CollideSphereVSSphere(m_pkBodyCopy1,m_pkBodyCopy2))
				{
					//cout<<"Stuck sphere VS sphere"<<endl;
					didpen = true;			
					fAtime = 0;
					break;					
				}
			}		

			
			didpen = true;			
			retry = true;
			fAtime /= m_fTimeSlice;
		
			continue;
		}
		else if(didpen)
			break;
		else
		{
			m_iNrOfTests += nroftests;
			return;
		}
		
	}	
	
	m_iNrOfTests += nroftests;
	
	if(didpen)
	{	
		if((pkCollission->fAtime > fAtime) || (pkCollission->fAtime == -1))
		{		
	
			//Tcs_collission* temp = new Tcs_collission;
			pkCollission->Clear();
			
			pkCollission->pkBody1 = pkBody1;
			pkCollission->pkBody2 = pkBody2;
		
			pkCollission->kNormals.push_back((m_pkBodyCopy1->m_kNewPos - m_pkBodyCopy2->m_kNewPos).Unit());
			pkCollission->kPositions.push_back(m_pkBodyCopy1->m_kNewPos - (pkCollission->kNormals[0] * m_pkBodyCopy1->m_fRadius));
			pkCollission->fAtime =	fAtime;
			
			//m_kCollissions.push_back(temp);					
		}
	}
}

bool Tcs::CollideSphereVSSphere(P_Tcs* pkBody1,P_Tcs* pkBody2)
{
	float d = pkBody1->m_kNewPos.DistanceTo(pkBody2->m_kNewPos) - (pkBody1->m_fRadius + pkBody2->m_fRadius);
				
	if(d <= 0)
		return true;
				
	return false;	
}


void Tcs::TestSphereVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime,Tcs_collission* pkCollission)
{
	bool retry = true;
	bool didpen = false;
	int nroftests = 0;
	
	//if body1 is the mesh just flip them,body2 shuld always be the mesh
	if(pkBody1->m_bPolygonTest)
	{
		P_Tcs* b2c = pkBody2;	
		pkBody2 = pkBody1;
		pkBody1 = b2c;
	}
	
	//make sure body2 have all the pointers else get angry and cry
	if(!pkBody2->m_pkVertex)
		return;		
	if(!pkBody2->m_pkFaces)
		return;
	
	
	while(retry && (fAtime >0.0) )
	{
		retry = false;
		nroftests++;

		if(fAtime < m_fAlmostZero)
			fAtime = 0;
					
		memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
		memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));				
		UpdateBodyVelnPos(m_pkBodyCopy1,fAtime);
		UpdateBodyVelnPos(m_pkBodyCopy2,fAtime);	
	
		if(CollideSphereVSMesh(m_pkBodyCopy1,m_pkBodyCopy2))
		{
			//if first penetration do a check at time 0
			if(!didpen)
			{
				memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
				memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));				
				UpdateBodyVelnPos(m_pkBodyCopy1,0);
				UpdateBodyVelnPos(m_pkBodyCopy2,0);	
						
				if(CollideSphereVSMesh(m_pkBodyCopy1,m_pkBodyCopy2))
				{
					didpen = true;			
					fAtime = 0;
					break;
				}
			}
							
			didpen = true;			
			retry = true;
			fAtime /=m_fTimeSlice;
		}
		else if(didpen)
			break;
		else
		{
			m_iNrOfTests += nroftests;
			return;		
		}
	}	

	
	m_iNrOfTests += nroftests;	
	if(didpen)
	{
		if((pkCollission->fAtime > fAtime) || (pkCollission->fAtime == -1))
		{
				
			//Tcs_collission* temp = new Tcs_collission ;
			pkCollission->Clear();
			
			pkCollission->pkBody1 = pkBody1;
			pkCollission->pkBody2 = pkBody2;	
			pkCollission->kNormals.push_back((m_pkBodyCopy1->m_kNewPos - m_kLastTestPos).Unit());
			pkCollission->kPositions.push_back(m_kLastTestPos);
			
			pkCollission->fAtime =	fAtime;
			
			//m_kCollissions.push_back(temp);				
		}
	}	
}

bool Tcs::CollideSphereVSMesh(P_Tcs* pkSphere,P_Tcs* pkMesh)
{
	if(!CollideSphereVSSphere(pkSphere,pkMesh))
		return false;
	
	
	Matrix4 kModelMatrix = pkMesh->GetModelMatrix();
	
	float closest = 99999999;
	Vector3 kClosestNormal;
	Vector3 kClosestPos;
	bool bHaveColided = false;
	
	Vector3 verts[3];	
	float d;


	for(unsigned int f=0;f<pkMesh->m_pkFaces->size();f++)
	{		 
		verts[0] = kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[0]]);
		verts[1] = kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[1]]);		
		verts[2] = kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[2]]);		
		
		if(m_iDebugGraph == 2)
		{
			//debug stuff
			m_pkRender->Line(verts[0],verts[1]);
			m_pkRender->Line(verts[1],verts[2]);		
			m_pkRender->Line(verts[2],verts[0]);				
		}		
		
		if(TestSphereVSPolygon(verts,pkSphere))
		{	
			d = pkSphere->m_kNewPos.DistanceTo(m_kLastTestPos);
		
			if( d < closest)
			{
				closest = d;
				bHaveColided = true;
				kClosestPos = m_kLastTestPos;
				kClosestNormal = m_kLastTestNormal;
			}
		}	
	}
	
	if(bHaveColided)
	{
		m_kLastTestPos = kClosestPos;
		m_kLastTestNormal = kClosestNormal;	
		return true;
	}
	
	return false;
}

bool Tcs::TestSphereVSPolygon(Vector3* kVerts,P_Tcs* pkSphere)
{
	Vector3 kPos1 = pkSphere->m_kNewPos;

	Vector3 V1 = kVerts[1] - kVerts[0];
	Vector3 V2 = kVerts[2] - kVerts[0];		
	Vector3 Normal= V1.Cross(V2);
	
	if(Normal.Length() == 0)
	{
		return false;
	}
	
	Normal.Normalize();
	Plane P;
	P.m_fD = -Normal.Dot(kVerts[0]);	
	P.m_kNormal = Normal;

	Vector3 kColPos;
	
	if(P.LineTest(kPos1 + (Normal * pkSphere->m_fRadius),kPos1 - (Normal * pkSphere->m_fRadius) ,&kColPos))
	{
		if(TestSides(kVerts,&Normal,kColPos))
		{
			m_kLastTestNormal = Normal;
			m_kLastTestPos = kColPos;
			return true;
		}
	}
	
	//do edge tests
	bool didcollide = false;
	float closest = 99999999;	
	int p1 ,p2;
	float d;
	
	for(int i = 0;i<3;i++)
	{
		switch(i)
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
	
		if(TestLineVSSphere(kVerts[p1], kVerts[p2],pkSphere))
		{
			d = pkSphere->m_kNewPos.DistanceTo(m_kLastTestPos);
			if( d < closest)
			{
				closest = d;
				kColPos = m_kLastTestPos;
				didcollide = true;
			}
		}		
	}
	
	if(didcollide)
	{
		m_kLastTestNormal = Normal;
		m_kLastTestPos = kColPos;
		return true;
	}
	
	
	//test verts
	d = 99999999;
	didcollide = false;
	for(int i = 0;i< 3;i++)
	{
		d = pkSphere->m_kNewPos.DistanceTo(kVerts[i]);
		if(d < pkSphere->m_fRadius)
			if(d < closest)
			{
				m_kLastTestNormal = Normal;
				m_kLastTestPos = kVerts[i];
				didcollide = true;
			}	
	}
	
	if(didcollide)
	{
		return true;
	}	
	
	return false;
}

void Tcs::TestMeshVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime,Tcs_collission* pkCollission)
{
	//make sure body 1 and 2 have all the pointers else get angry and cry
	if( !pkBody2->m_pkVertex || !pkBody2->m_pkFaces || !pkBody1->m_pkVertex || !pkBody1->m_pkFaces)
		return;		

			
	bool retry = 			true;
	bool didpen = 			false;
	int nroftests = 		1;
	float fLastColTime = fAtime;
	float fCT = 			0;
	float	fLastNoColTime = 0;
	float fStep = 			fAtime / 4.0;
	
	//first do a check att max time
	memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
	memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));						
	UpdateBodyVelnPos(m_pkBodyCopy1,fAtime);
	UpdateBodyVelnPos(m_pkBodyCopy2,fAtime);	
	m_iNrOfTests++;	
	if(!CollideMeshVSMesh3(m_pkBodyCopy1,m_pkBodyCopy2,NULL))
		return;
	
	while(retry && (fCT <= fAtime) )
	{
		retry = false;
		
		//check if we already passed the first known collission
		if(pkCollission->fAtime != -1)
			if(fCT > pkCollission->fAtime)
				return;
				
	
		memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
		memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));						
		UpdateBodyVelnPos(m_pkBodyCopy1,fCT);
		UpdateBodyVelnPos(m_pkBodyCopy2,fCT);			
			
		m_iNrOfTests++;		
		if(CollideMeshVSMesh3(m_pkBodyCopy1,m_pkBodyCopy2,NULL))
		{			
			fLastColTime = fCT;
			didpen = true;
			break;
		}
		else
		{
			fLastNoColTime = fCT;
			retry = true;
			fCT += fStep;
		}		
	}
	
	/*
	while(retry && (fAtime > 0) )
	{
		retry = false;
		nroftests++;
			
		if(fAtime < m_fAlmostZero)
		{
			fAtime = 0;
		}
		
		memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
		memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));		
				
		UpdateBodyVelnPos(m_pkBodyCopy1,fAtime);
		UpdateBodyVelnPos(m_pkBodyCopy2,fAtime);	
	
		if(CollideMeshVSMesh3(m_pkBodyCopy1,m_pkBodyCopy2,NULL))
		{
			//if first penetration do a check at time 0
			if(!didpen)
			{
				memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
				memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));				
				UpdateBodyVelnPos(m_pkBodyCopy1,0);
				UpdateBodyVelnPos(m_pkBodyCopy2,0);	
						
				if(CollideMeshVSMesh3(pkBody1,pkBody2,NULL))
				{
					didpen = true;			
					fAtime = 0;
					fLastColTime = 0;
					break;
				}
			}	 
			
			didpen = true;			
			retry = true;
			fLastColTime  = fAtime;
			fAtime /= m_fTimeSlice;
		}
		else if(didpen)
			break;
		else
		{			
			m_iNrOfTests += nroftests; 
			return;
		}	
	}	
	*/
	
	//m_iNrOfTests += nroftests; 
	
	if(didpen)
	{	
		if((pkCollission->fAtime > fLastNoColTime) || (pkCollission->fAtime == -1))
		{	
		
			//Tcs_collission* pkTempCol = new Tcs_collission;	
			pkCollission->Clear();
		
			//now collect collission points for col time
			memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
			memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));						
			UpdateBodyVelnPos(m_pkBodyCopy1,fLastColTime);
			UpdateBodyVelnPos(m_pkBodyCopy2,fLastColTime);		
			m_iNrOfTests++;			
			if(!CollideMeshVSMesh3(m_pkBodyCopy1,m_pkBodyCopy2,pkCollission))
			{
				cout<<"FUCKING DAM SHIT SOMEHING IS DAAAMN WRONG HERE"<<endl;
				return;
			}
		
			pkCollission->pkBody1 = pkBody1;
			pkCollission->pkBody2 = pkBody2;		
			pkCollission->fAtime =	fLastNoColTime;
		}
	}	
}

bool Tcs::CollideMeshVSMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,Tcs_collission* pkTempCol) 
{
//	if(!CollideSphereVSSphere(pkBody1,pkBody2))
//		return false;
		
		
	Matrix4 kModelMatrix1 = pkBody1->GetModelMatrix();
	Matrix4 kModelMatrix2 = pkBody2->GetModelMatrix();

	bool bHaveCleared = false;		
	bool bHaveColided = false;	
	Vector3 verts1[3];	
	Vector3 verts2[3];	
	Plane P;
	Vector3 Normal;

	for(unsigned int f=0;f<pkBody1->m_pkFaces->size();f++)
	{
		verts1[0] = kModelMatrix1.VectorTransform((*pkBody1->m_pkVertex)[(*pkBody1->m_pkFaces)[f].iIndex[0]]);
		verts1[1] = kModelMatrix1.VectorTransform((*pkBody1->m_pkVertex)[(*pkBody1->m_pkFaces)[f].iIndex[1]]);		
		verts1[2] = kModelMatrix1.VectorTransform((*pkBody1->m_pkVertex)[(*pkBody1->m_pkFaces)[f].iIndex[2]]);		
	
		if(verts1[0] == verts1[1])
			continue;		
		if(verts1[0] == verts1[2])
			continue;
		if(verts1[1] == verts1[2])
			continue;
			
										
		Normal= ((verts1[1] - verts1[0]).Cross(verts1[2] - verts1[0])).Unit();				
		P.Set(Normal,verts1[0]);
		
		if(m_iDebugGraph == 2)
		{
			//debug stuff
			m_pkRender->Line(verts1[0],verts1[1]);
			m_pkRender->Line(verts1[1],verts1[2]);		
			m_pkRender->Line(verts1[2],verts1[0]);				
			m_pkRender->Line(verts1[0],verts1[0] + Normal);		
			m_pkRender->Line(verts1[1],verts1[1] + Normal);				
			m_pkRender->Line(verts1[2],verts1[2] + Normal);
		}
		
		
		for(unsigned int g=0;g<pkBody2->m_pkFaces->size();g++)
		{
			verts2[0] = kModelMatrix2.VectorTransform((*pkBody2->m_pkVertex)[(*pkBody2->m_pkFaces)[g].iIndex[0]]);
			verts2[1] = kModelMatrix2.VectorTransform((*pkBody2->m_pkVertex)[(*pkBody2->m_pkFaces)[g].iIndex[1]]);		
			verts2[2] = kModelMatrix2.VectorTransform((*pkBody2->m_pkVertex)[(*pkBody2->m_pkFaces)[g].iIndex[2]]);		
			
			if(verts2[0] == verts2[1])
				continue;		
			if(verts2[0] == verts2[2])
				continue;
			if(verts2[1] == verts2[2])
				continue;					
							
			if(m_iDebugGraph == 2)
			{
				//debug stuff
				m_pkRender->Line(verts2[0],verts2[1]);
				m_pkRender->Line(verts2[1],verts2[2]);		
				m_pkRender->Line(verts2[2],verts2[0]);				
			}
			
			
			int i1,i2;
			for(int i = 0;i<3;i++)
			{
				switch(i)
				{
					case 0:
						i1 = 0;
						i2 = 1; 
						break;
		
					case 1:
						i1 = 1;
						i2 = 2; 
						break;
		
					case 2:
						i1 = 2;
						i2 = 0; 
						break;
				}			
				
/*				if(TestLineVSPolygon(verts1,&verts2[i1],&verts2[i2],&P,NULL))
				{
					if(pkTempCol)
					{
						if(!bHaveCleared)
						{
							bHaveCleared = true;
							pkTempCol->kNormals.clear();
							pkTempCol->kPositions.clear();
						}
						
						
						pkTempCol->kPositions.push_back(m_kLastTestPos);
						pkTempCol->kNormals.push_back(m_kLastTestNormal);
						
						bHaveColided = true;	
					}
					else
						return true;
				}*/
			}			
		}
	}
	
	if(bHaveColided)
		return true;
	
	return false;
}

bool Tcs::CollideMeshVSMesh3(P_Tcs* pkBody1,P_Tcs* pkBody2,Tcs_collission* pkTempCol) 
{
	Matrix4 kModelMatrix1 = pkBody1->GetModelMatrix();
	Matrix4 kModelMatrix2 = pkBody2->GetModelMatrix();

	//bool bHaveCleared = false;		
	bool bHaveColided = false;	
	static Vector3 verts1[3];	
	static Vector3 verts2[3];	
	static Plane P1;
	static Plane P2;
	static Vector3 Normal1;
	static Vector3 Normal2;
	static int i1,i2;

	for(unsigned int f=0;f<pkBody1->m_pkFaces->size();f++)
	{
		verts1[0] = kModelMatrix1.VectorTransform((*pkBody1->m_pkVertex)[(*pkBody1->m_pkFaces)[f].iIndex[0]]);
		verts1[1] = kModelMatrix1.VectorTransform((*pkBody1->m_pkVertex)[(*pkBody1->m_pkFaces)[f].iIndex[1]]);		
		verts1[2] = kModelMatrix1.VectorTransform((*pkBody1->m_pkVertex)[(*pkBody1->m_pkFaces)[f].iIndex[2]]);		
		
		if(verts1[0] == verts1[1])
			continue;		
		if(verts1[0] == verts1[2])
			continue;
		if(verts1[1] == verts1[2])
			continue;
			
		if(m_iDebugGraph == 2)
		{
			//debug stuff
			m_pkRender->Line(verts1[0],verts1[1]);
			m_pkRender->Line(verts1[1],verts1[2]);		
			m_pkRender->Line(verts1[2],verts1[0]);		
		}					
			
										
		Normal1= ((verts1[1] - verts1[0]).Cross(verts1[2] - verts1[0])).Unit();				
		P1.Set(Normal1,verts1[0]);
		

		
		
		for(unsigned int g=0;g<pkBody2->m_pkFaces->size();g++)
		{
			verts2[0] = kModelMatrix2.VectorTransform((*pkBody2->m_pkVertex)[(*pkBody2->m_pkFaces)[g].iIndex[0]]);
			verts2[1] = kModelMatrix2.VectorTransform((*pkBody2->m_pkVertex)[(*pkBody2->m_pkFaces)[g].iIndex[1]]);		
			verts2[2] = kModelMatrix2.VectorTransform((*pkBody2->m_pkVertex)[(*pkBody2->m_pkFaces)[g].iIndex[2]]);		
			
			if(verts2[0] == verts2[1])
				continue;		
			if(verts2[0] == verts2[2])
				continue;
			if(verts2[1] == verts2[2])
				continue;					

			if(m_iDebugGraph == 2)
			{
				//debug stuff
				m_pkRender->Line(verts1[0],verts1[1]);
				m_pkRender->Line(verts1[1],verts1[2]);		
				m_pkRender->Line(verts1[2],verts1[0]);		
			}						
				
			Normal2= ((verts2[1] - verts2[0]).Cross(verts2[2] - verts2[0])).Unit();				
			P2.Set(Normal2,verts2[0]);

			//body1 vs body2			
			for(int i = 0;i<3;i++)
			{
				switch(i)
				{
					case 0:
						i1 = 0;
						i2 = 1; 
						break;
		
					case 1:
						i1 = 1;
						i2 = 2; 
						break;
		
					case 2:
						i1 = 2;
						i2 = 0; 
						break;
				}			
				
				if(pkTempCol)				
				{
					if(TestLineVSPolygon(verts1,&verts2[i1],&verts2[i2],&P1,Normal2,GetNeighbourFaceNormal(verts2[i1],verts2[i2],g,pkBody2)))
					{
						pkTempCol->kPositions.push_back(m_kLastTestPos);
						pkTempCol->kNormals.push_back(m_kLastTestNormal);
							
						bHaveColided = true;	
					}
					
					if(TestLineVSPolygon(verts2,&verts1[i1],&verts1[i2],&P2,Normal1,GetNeighbourFaceNormal(verts1[i1],verts1[i2],f,pkBody1)))
					{							
						pkTempCol->kPositions.push_back(m_kLastTestPos);
						pkTempCol->kNormals.push_back(-m_kLastTestNormal);
							
						bHaveColided = true;	
					}
					
				}
				else
				{
					if(TestLineVSPolygonNoNormal(verts1,&verts2[i1],&verts2[i2],&P1))
						return true;
					
					if(TestLineVSPolygonNoNormal(verts2,&verts1[i1],&verts1[i2],&P2))
						return true;
						
				}
			}			
		}
	}
	
	if(bHaveColided)
		return true;
	
	return false;
}


bool Tcs::TestLineVSPolygon(Vector3* pkPolygon,Vector3* pkPos1,Vector3* pkPos2,Plane* pkPlane,const Vector3& kNormal1,const Vector3& kNormal2)
{

	static float fMinDist = 0.01;

	if(pkPlane->LineTest(*pkPos1,*pkPos2,&m_kLastTestPos))
	{
		if(TestSides(pkPolygon,&(pkPlane->m_kNormal),m_kLastTestPos))
		{
			//m_kLastTestNormal = -pkPlane->m_kNormal;		
			//return true;
			
			if( m_kLastTestPos.DistanceTo(*pkPos1) < fMinDist ||
				 m_kLastTestPos.DistanceTo(*pkPos2) < fMinDist /*|| 
				 m_kLastTestPos.DistanceTo(pkPolygon[0]) < fMinDist ||
				 m_kLastTestPos.DistanceTo(pkPolygon[1]) < fMinDist ||
				 m_kLastTestPos.DistanceTo(pkPolygon[2]) < fMinDist*/)
			{			
				//vertex VS face
				m_kLastTestNormal = -pkPlane->m_kNormal;
				return true;
			}
			else
			{
				//edge VS edge
			
				Vector3 L = (*pkPos2 - *pkPos1);
			
				//calculate normals
				Vector3 n1 = L.Cross(pkPolygon[0] - pkPolygon[1]).Unit();
				Vector3 n2 = L.Cross(pkPolygon[1] - pkPolygon[2]).Unit();
				Vector3 n3 = L.Cross(pkPolygon[2] - pkPolygon[0]).Unit();
										
				//calculate distance betwen edges				
				float f1 = (pkPolygon[0] - *pkPos1).Proj(n1).Length();
				float f2 = (pkPolygon[1] - *pkPos1).Proj(n2).Length();
				float f3 = (pkPolygon[2] - *pkPos1).Proj(n3).Length();
								
				
				if((f1 < f2) && (f1 < f3))
				{
					m_kLastTestNormal = n1;
				};
				if((f2 < f1) && (f2 < f3))
				{
					m_kLastTestNormal = n2;
				};
				if((f3 < f1) && (f3 < f2))
				{
					m_kLastTestNormal = n3;
				};			
				
				
				//if(kNormal1 != kNormal2) cout<<"bah"<<endl;				
				//cout<<"huma:"<<m_kLastTestNormal.Dot(kNormal1)<<" - "<<m_kLastTestNormal.Dot(kNormal2)<<endl;;
				
				if( 	(m_kLastTestNormal.Dot(kNormal1) > 0) 
					&& (m_kLastTestNormal.Dot(kNormal2) > 0)) 
				{
					return true;				
				}
				
				m_kLastTestNormal = -m_kLastTestNormal;									
				if( 	(m_kLastTestNormal.Dot(kNormal1) > 0) 
					&& (m_kLastTestNormal.Dot(kNormal2) > 0)) 
				{
					return true;				
				}
				
				/*
				// pi / 2  = 1.570796327				
				//compare to face normal to chose wich direction the normal shuld have
				if(   (m_kLastTestNormal.Angle( kNormal1) < 1.570796327 ) 
					&& (m_kLastTestNormal.Angle( kNormal2) < 1.570796327 ) )  
				{			
					return true;
				}
				
					
				//invert normal and try again
				m_kLastTestNormal = -m_kLastTestNormal;									
				if(   (m_kLastTestNormal.Angle( kNormal1) < 1.570796327 ) 
					&& (m_kLastTestNormal.Angle( kNormal2) < 1.570796327 ) )  
				{			
					return true;
				}
				*/	
								
				return false;		
			}			
		}
	}
	
	return false;
}

Vector3 Tcs::GetNeighbourFaceNormal(const Vector3& kVert1,const Vector3& kVert2,const int& iCurrentFace,P_Tcs* pkBody)
{
	Matrix4 kModelMatrix2 = pkBody->GetModelMatrix();
	static Vector3 verts[3];
	static Vector3 kNormal;	

	for(unsigned int g=0;g<pkBody->m_pkFaces->size();g++)
	{
		if(g == iCurrentFace)
			continue;
		
		verts[0] = kModelMatrix2.VectorTransform((*pkBody->m_pkVertex)[(*pkBody->m_pkFaces)[g].iIndex[0]]);
		verts[1] = kModelMatrix2.VectorTransform((*pkBody->m_pkVertex)[(*pkBody->m_pkFaces)[g].iIndex[1]]);		
		verts[2] = kModelMatrix2.VectorTransform((*pkBody->m_pkVertex)[(*pkBody->m_pkFaces)[g].iIndex[2]]);		
	
		//edge 1
		if((verts[0] == kVert1 && verts[1] == kVert2) ||
		 	(verts[1] == kVert1 && verts[0] == kVert2))
		{
			return ((verts[1] - verts[0]).Cross(verts[2] - verts[0])).Unit();;
		}
		
		//edge 2		
		if((verts[1] == kVert1 && verts[2] == kVert2) ||
		 	(verts[2] == kVert1 && verts[1] == kVert2))
		{
			return ((verts[1] - verts[0]).Cross(verts[2] - verts[0])).Unit();;
		}
		
		//edge 3
		if((verts[2] == kVert1 && verts[0] == kVert2) ||
		 	(verts[0] == kVert1 && verts[2] == kVert2))
		{
			return ((verts[1] - verts[0]).Cross(verts[2] - verts[0])).Unit();;
		}		
	}

	//cout<<"found: "<<found<<endl;
	//cout<<"match: "<<match<<endl;
	//cout<<"warning could nto find neightbour"<<endl;
	
	//no neighbour found, returns known normal
	verts[0] = kModelMatrix2.VectorTransform((*pkBody->m_pkVertex)[(*pkBody->m_pkFaces)[iCurrentFace].iIndex[0]]);
	verts[1] = kModelMatrix2.VectorTransform((*pkBody->m_pkVertex)[(*pkBody->m_pkFaces)[iCurrentFace].iIndex[1]]);		
	verts[2] = kModelMatrix2.VectorTransform((*pkBody->m_pkVertex)[(*pkBody->m_pkFaces)[iCurrentFace].iIndex[2]]);		
	
	return ((verts[1] - verts[0]).Cross(verts[2] - verts[0])).Unit();;
}


bool Tcs::TestLineVSPolygonNoNormal(Vector3* pkPolygon,Vector3* pkPos1,Vector3* pkPos2,Plane* pkPlane)
{
	static Vector3 kTestPos;
	
	if(pkPlane->LineTest(*pkPos1,*pkPos2,&kTestPos))
		if(TestSides(pkPolygon,&(pkPlane->m_kNormal),kTestPos))
			return true;
		
		
	return false;
}

void Tcs::ResetForces()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
		if(m_kBodys[i]->m_bStatic)
			continue;
							
		m_kBodys[i]->ClearExternalForces();
		//m_kBodys[i]->m_kExternalLinearForce.Set(0,0,0);
		//m_kBodys[i]->m_kExternalRotForce.Set(0,0,0);	3	
	}
}


void Tcs::TryToSleep(P_Tcs* pkBody1,P_Tcs* pkBody2)
{
	if((pkBody1->m_kLinearVelocity.Length() < m_fSleepVel) &&
		(pkBody1->m_kRotVelocity.Length() <  m_fSleepVel/2) &&
		(pkBody2->m_kLinearVelocity.Length() < m_fSleepVel) &&
		(pkBody2->m_kRotVelocity.Length() <  m_fSleepVel/2) )		
	{
		pkBody1->Sleep();
		pkBody2->Sleep();
		
		if(pkBody1->m_bSleeping)
			if(pkBody1->m_bDisableOnSleep)
			{
				pkBody1->Disable();
				pkBody1->GetObject()->DeleteProperty("P_Tcs");;
			}

		if(pkBody2->m_bSleeping)
			if(pkBody2->m_bDisableOnSleep)
			{
				pkBody2->Disable();
				pkBody2->GetObject()->DeleteProperty("P_Tcs");
			}
	}
}



bool Tcs::CollideMeshVSMesh2(P_Tcs* pkBody1,P_Tcs* pkBody2)
{
	if(!CollideSphereVSSphere(pkBody1,pkBody2))
		return false;
		
		
	bool bHaveCleared = false;	
		

	Matrix4 kModelMatrix1 = pkBody1->GetModelMatrix();
	Matrix4 kModelMatrix2 = pkBody2->GetModelMatrix();

	bool bHaveColided = false;
	
	Vector3 verts1[3];	
	Vector3 verts2[3];	
	
	float fClosest=1;
	Vector3 kPos;
	Vector3 kNormal;


	for(unsigned int f=0;f<pkBody1->m_pkFaces->size();f++)
	{
		verts1[0] = kModelMatrix1.VectorTransform((*pkBody1->m_pkVertex)[(*pkBody1->m_pkFaces)[f].iIndex[0]]);
		verts1[1] = kModelMatrix1.VectorTransform((*pkBody1->m_pkVertex)[(*pkBody1->m_pkFaces)[f].iIndex[1]]);		
		verts1[2] = kModelMatrix1.VectorTransform((*pkBody1->m_pkVertex)[(*pkBody1->m_pkFaces)[f].iIndex[2]]);		
	
		if(verts1[0] == verts1[1])
			continue;		
		if(verts1[0] == verts1[2])
			continue;
		if(verts1[1] == verts1[2])
			continue;
			
		
		
		for(unsigned int g=0;g<pkBody2->m_pkFaces->size();g++)
		{
			verts2[0] = kModelMatrix2.VectorTransform((*pkBody2->m_pkVertex)[(*pkBody2->m_pkFaces)[g].iIndex[0]]);
			verts2[1] = kModelMatrix2.VectorTransform((*pkBody2->m_pkVertex)[(*pkBody2->m_pkFaces)[g].iIndex[1]]);		
			verts2[2] = kModelMatrix2.VectorTransform((*pkBody2->m_pkVertex)[(*pkBody2->m_pkFaces)[g].iIndex[2]]);		
			
			if(verts1[0] == verts1[1])
				continue;		
			if(verts1[0] == verts1[2])
				continue;
			if(verts1[1] == verts1[2])
				continue;
			
			if(TestPolygonVSPolygon(verts1,verts2))
			{
				if(m_fLastTestDist < fClosest)
				{
					bHaveColided = true;
				
					fClosest = m_fLastTestDist;
					
					kPos = m_kLastTestPos;
					kNormal = m_kLastTestNormal;
				}
			}
		
		
		}
	}
	
	if(bHaveColided)
		return true;
	
	return false;
}


bool Tcs::TestPolygonVSPolygon(Vector3* pkPolygon1,Vector3* pkPolygon2)
{
	Plane kPlane1;	
	Plane kPlane2;	
	
	Vector3 kNormal1= ((pkPolygon1[1] - pkPolygon1[0]).Cross(pkPolygon1[2] - pkPolygon1[0])).Unit();				
	Vector3 kNormal2= ((pkPolygon2[1] - pkPolygon2[0]).Cross(pkPolygon2[2] - pkPolygon2[0])).Unit();				
	
	kPlane1.Set(kNormal1,pkPolygon1[0]);
	kPlane2.Set(kNormal2,pkPolygon2[0]);
	
		
	m_fLastTestDist = 1;	
	int i1,i2;	
	for(int i = 0;i<6;i++)
	{
		switch(i)
		{
			case 0:
			case 3:
				i1 = 0;
				i2 = 1; 
				break;

			case 1:
			case 4:
				i1 = 1;
				i2 = 2; 
				break;

			case 2:
			case 5:
				i1 = 2;
				i2 = 0; 
				break;
										
		}
	
		float d;
		Vector3 kPos;
		
		if(i <= 2)
			d = LineInsidePolygon(pkPolygon2,&kPlane2,&pkPolygon1[i1],&pkPolygon1[i2],&kPos);
		else
			d = LineInsidePolygon(pkPolygon1,&kPlane1,&pkPolygon2[i1],&pkPolygon2[i2],&kPos);
			
		if(d < m_fLastTestDist)
		{
			m_fLastTestDist = d;
			m_kLastTestPos = kPos;
			
			if(i <= 2)			
				m_kLastTestNormal = kPlane2.m_kNormal;
			else
				m_kLastTestNormal = -kPlane1.m_kNormal;
		}
	}

	if(m_fLastTestDist != 1)
	{
		return true;
	}
	
	return false;
}

float Tcs::LineInsidePolygon(Vector3* pkPolgyon,Plane* pkPlane,Vector3* pkLinePos1,Vector3* pkLinePos2,Vector3* pkColPos)
{
	if(pkPlane->LineTest(*pkLinePos1,*pkLinePos2,pkColPos))
	{
		if(TestSides(pkPolgyon,&(pkPlane->m_kNormal),*pkColPos))
		{
			float d1 = pkPlane->PointInside(*pkLinePos1);
			float d2 = pkPlane->PointInside(*pkLinePos2);
					
			if(d1 < d2)
			{
				//*pkColPos = *pkLinePos1;
				return d1;
			}
			else
			{
				//*pkColPos = *pkLinePos2;
				return d2;
			}
		}		
	}
	
	return 1;
}

/*
void Tcs::GenerateModelMatrix(P_Tcs* pkMesh)
{
	m_kModelMatrix.Identity();
	m_kModelMatrix.Scale(pkMesh->m_fScale,pkMesh->m_fScale,pkMesh->m_fScale);
	m_kModelMatrix*=pkMesh->GetObject()->GetWorldOriM();

}
*/
