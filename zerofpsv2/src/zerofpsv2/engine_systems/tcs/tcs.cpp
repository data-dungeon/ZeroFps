#include "tcs.h"
#include "../propertys/p_tcs.h"
#include "../common/heightmap.h"
	
Tcs::Tcs(): ZFSubSystem("Tcs")
{
	Logf("zerofps","Tiny Collission system created");

 	m_fMaxDelay = 		0.05;
	m_fMinForce = 		0.2;
	m_fAlmostZero = 	0.001;
	m_fSleepVel = 		0.2;
	m_fMaxVel = 		10.0;
	
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
	//cout<<"boddy added"<<endl;
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
			
			//cout<<"body removed"<<endl;			
			return;
		}
	}
}



void Tcs::Update(float fAlphaTime)
{
	if(m_kBodys.empty())
		return;
	
/*				
	//make sure alpha time is not to small
	if(fAlphaTime > 0.033) 
	{
		cout<<"slow:"<<fAlphaTime<<endl;
		fAlphaTime = 0.033; 
	}
*/			
//		cout<<"slow:"<<fAlphaTime<<endl;
	
		
	float fStartTime = m_pkZeroFps->GetTicks();
	float fRTime = fAlphaTime;
	m_iNrOfCollissions = 0;
	m_iNrOfTests = 0;
		
	//synd all bodys to entitys
	SyncBodys();
	
	//make sure there is no collissions	
	ClearCollissions();
	
	//calculate current forces
	UpdateForces();
	
	
	
	while(fRTime > 0)
	{
		//do collissiontests
		UpdateCollissions(fRTime);
	
		//did any collission ocur?
		if(m_kCollissions.size() > 0)
		{
			m_iNrOfCollissions++;
		
			//get next closest collission	
			Tcs_collission* pkCol = FindNextCollission();			
			
			//update all objects up to current time
			UpdateAllVelnPos(pkCol->fAtime);			
			
			//handle collission
			HandleCollission(pkCol);
				
			//set new Resttime			
			fRTime -= pkCol->fAtime;			
			
			//clear all collissions
			ClearCollissions();

			if((m_pkZeroFps->GetTicks() - fStartTime) > m_fMaxDelay)
				break;
								
		}
		//no collissions, just update time and continue
		else 
		{
			UpdateAllVelnPos(fRTime);	
			fRTime = 0;
		}	
	}
	
	//update all character ground line tests
	UpdateLineTests(fAlphaTime);
	
	//synd all entitys to bodys
	SyncEntitys();
	
	//clear all forces
//	ResetForces();
	
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

	//cout<<"collission points:"<<pkCol->kPositions.size()<<endl;	
	int iNrOfPos = pkCol->kPositions.size();
 	//iNrOfPos = 1;
	
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
			m_pkRender->Sphere(pkCol->kPositions[i],0.1,1,Vector3(0,1,0),false);
			m_pkRender->SetColor(Vector3(1,0,1));
			m_pkRender->Line(pkCol->kPositions[i],pkCol->kPositions[i] + pkCol->kNormals[i]);		
			m_pkRender->SetColor(Vector3(1,1,1));
		}
		
		if(!m_iHandleCollission)
			continue;

		//Vector3 kRelVel = kLinearRelVel;		
		//Vector3 kRelVel = kLinearRelVel + (((pkCol->kPositions[i] - pkCol->pkBody1->m_kNewPos).Unit()*0.25).Cross(kRotVel1) - ((pkCol->kPositions[i] - pkCol->pkBody2->m_kNewPos).Unit()*0.25).Cross(kRotVel2));		
		Vector3 kRelVel = kLinearRelVel + (-kRotVel1.Cross(pkCol->kPositions[i] - pkCol->pkBody1->m_kNewPos) - -kRotVel2.Cross(pkCol->kPositions[i] - pkCol->pkBody2->m_kNewPos));		
		//Vector3 kRelVel = pkCol->pkBody1->GetVel(pkCol->kPositions[i],false) - pkCol->pkBody2->GetVel(pkCol->kPositions[i],false);
		
		if(kRelVel.Length() == 0)
		{
			//this shuld not hapen...but it does...=(
			//TryToSleep(pkCol->pkBody1,pkCol->pkBody2);		
			return;			
		}
		
		Vector3 kTangent = (pkCol->kNormals[i].Cross(kRelVel.Unit())).Cross(pkCol->kNormals[i]);
			
			
						
		float j  = (-(1+fBounce) * (kRelVel * pkCol->kNormals[i])) /
					  ( (pkCol->kNormals[i] * pkCol->kNormals[i]) *
					  ( 1/fMass1 + 1/fMass2)); 	

		/*			  
		if(j<0)
		{
			cout<<"hora"<<endl;
			return;
		}
		*/	
		
		
		//make sure the impact force is not to small					  
		if(j < m_fMinForce)
			j = m_fMinForce;  
		
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
		//m_kBodys[i]->m_kLinearVelocity = m_kBodys[i]->GetObject()->GetVel();				
		m_kBodys[i]->m_kNewRotation = m_kBodys[i]->GetObject()->GetLocalRotM();
	
	//	cout<<"wieard:"<<m_kBodys[i]->m_kNewPos.x<<" "<<m_kBodys[i]->m_kNewPos.y<<" "<<m_kBodys[i]->m_kNewPos.z<<endl;
			
	}
}

void Tcs::UpdateForces()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
		if(m_kBodys[i]->m_bStatic || m_kBodys[i]->m_bSleeping)
			continue;

				
		// LINEAR FORCE / ACCLERERATION			
			m_kBodys[i]->m_kLinearForce.Set(0,0,0);
		
			//add external linear force
			m_kBodys[i]->m_kLinearForce += m_kBodys[i]->m_kExternalLinearForce; 
			
			//Apply walk force
			m_kBodys[i]->m_kLinearForce+=m_kBodys[i]->m_kWalkVel;
			
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
			
				if(!TestMotionSpheres(m_kBodys[B1],m_kBodys[B2],fAtime))
					continue;
					
				if(m_kBodys[B1]->m_bPolygonTest && m_kBodys[B2]->m_bPolygonTest)
				{
					TestMeshVsMesh(m_kBodys[B1],m_kBodys[B2],fAtime);
				}
				else if(m_kBodys[B1]->m_bPolygonTest || m_kBodys[B2]->m_bPolygonTest)
				{				
					TestSphereVsMesh(m_kBodys[B1],m_kBodys[B2],fAtime);
				}
				else
				{
					TestSphereVsSphere(m_kBodys[B1],m_kBodys[B2],fAtime);
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

bool Tcs::TestMotionSpheres(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime)
{
	memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
	memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));
		
	UpdateBodyVelnPos(m_pkBodyCopy1,fAtime);
	UpdateBodyVelnPos(m_pkBodyCopy2,fAtime);	


	Vector3 kMedPos1 = (pkBody1->m_kNewPos + m_pkBodyCopy1->m_kNewPos) / 2.0;
	Vector3 kMedPos2 = (pkBody2->m_kNewPos + m_pkBodyCopy2->m_kNewPos) / 2.0;

	float d = kMedPos1.DistanceTo(kMedPos2) - (pkBody1->m_fRadius + pkBody2->m_fRadius);

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


Tcs_collission* Tcs::FindNextCollission()
{
	if(m_kCollissions.size() == 0)
		return NULL;
		
	float fClosest = 9999999999.0;
	Tcs_collission* pkClosest = NULL;
	
	for(unsigned int i=0;i<m_kCollissions.size();i++)
	{
		if(m_kCollissions[i]->fAtime < fClosest)
		{
			fClosest = m_kCollissions[i]->fAtime;
			pkClosest = m_kCollissions[i];
		}
	}
	
	return pkClosest;
}




void Tcs::TestSphereVsSphere(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime)
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
			fAtime /=1.5;
		
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
		
		
		Tcs_collission* temp = new Tcs_collission;
		temp->Clear();
		
		temp->pkBody1 = pkBody1;
		temp->pkBody2 = pkBody2;
	
		temp->kNormals.push_back((m_pkBodyCopy1->m_kNewPos - m_pkBodyCopy2->m_kNewPos).Unit());
		temp->kPositions.push_back(m_pkBodyCopy1->m_kNewPos - (temp->kNormals[0] * m_pkBodyCopy1->m_fRadius));
		temp->fAtime =	fAtime;
		
		m_kCollissions.push_back(temp);		
				
	}

}

bool Tcs::CollideSphereVSSphere(P_Tcs* pkBody1,P_Tcs* pkBody2)
{
	float d = pkBody1->m_kNewPos.DistanceTo(pkBody2->m_kNewPos) - (pkBody1->m_fRadius + pkBody2->m_fRadius);
				
	if(d <= 0)
		return true;
				
	return false;	
}


void Tcs::TestSphereVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime)
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
	
/*		
	int iTests = 4;
	float fMinDiff = 0.01;
	float fTimeDiff = fAtime;
	while(retry)
	{
		nroftests++;
	
		memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
		memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));				
		UpdateBodyVelnPos(m_pkBodyCopy1,fAtime);
		UpdateBodyVelnPos(m_pkBodyCopy2,fAtime);	
		
		if(CollideSphereVSMesh(m_pkBodyCopy1,m_pkBodyCopy2))
		{
			fTimeDiff /= 2.0;
			fAtime -= fTimeDiff;
			
			
			didpen = true;
		}	
		else
		{
			if(!didpen)
			{
				m_iNrOfTests += nroftests;
				return;
			}
				
			if(nroftests >= iTests)
				break;
		
			fTimeDiff /= 2.0;
			fAtime += fTimeDiff;
		}
	}	
*/	
	
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
					//cout<<"Stuck Object detected"<<endl;
					didpen = true;			
					fAtime = 0;
					break;
					//return;
				}
			}
							
			didpen = true;			
			retry = true;
			fAtime /=2;
			//fAtime -= fTD;
		
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
	
		Tcs_collission* temp = new Tcs_collission ;
		temp->Clear();
		
		temp->pkBody1 = pkBody1;
		temp->pkBody2 = pkBody2;
	
		temp->kNormals.push_back((m_pkBodyCopy1->m_kNewPos - m_kLastTestPos).Unit());
		temp->kPositions.push_back(m_kLastTestPos);
		
		temp->fAtime =	fAtime;
		
		m_kCollissions.push_back(temp);		
		
		
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

void Tcs::TestMeshVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime)
{
	bool retry = true;
	bool didpen = false;
	int nroftests = 0;
	float fLastColTime = 0;
	
	//make sure body 1 and 2 have all the pointers else get angry and cry
	if( !pkBody2->m_pkVertex || !pkBody2->m_pkFaces || !pkBody1->m_pkVertex || !pkBody1->m_pkFaces)
	{
		return;		
	}
	
	//the smalest object shuld be body2 for best result
	if(pkBody2->m_fRadius > pkBody1->m_fRadius)
	{
		P_Tcs* pkBakup = pkBody1;
	
		pkBody1 = pkBody2;
		pkBody2 = pkBakup;
	}
	
	
	while(retry && (fAtime > 0) )
	{
	
		retry = false;
		nroftests++;
			
		if(fAtime < m_fAlmostZero)
			fAtime = 0;

		
		memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
		memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));		
				
		UpdateBodyVelnPos(m_pkBodyCopy1,fAtime);
		UpdateBodyVelnPos(m_pkBodyCopy2,fAtime);	
	
		if(CollideMeshVSMesh(m_pkBodyCopy1,m_pkBodyCopy2,NULL))
		{
/*			//if first penetration do a check at time 0
			if(!didpen)
			{
				if(CollideMeshVSMesh(pkBody1,pkBody2,pkTempCol))
				{
					cout<<"Stuck mesh VS mesh detected"<<endl;
					//fAtime = 1;
					didpen = true;
					
					break;
					
					//return;
				}
			}	 
*/			

			
			didpen = true;			
			retry = true;
			fLastColTime  = fAtime;
			fAtime /= 2;
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
		Tcs_collission* pkTempCol = new Tcs_collission;	
		pkTempCol->Clear();
	
		//now collect collission points for col time
		memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
		memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));						
		UpdateBodyVelnPos(m_pkBodyCopy1,fLastColTime);
		UpdateBodyVelnPos(m_pkBodyCopy2,fLastColTime);		
		if(!CollideMeshVSMesh(m_pkBodyCopy1,m_pkBodyCopy2,pkTempCol))
			cout<<"FUCKING DAM SHIT SOMEHING IS DAAAMN WRONG HERE"<<endl;
	
	
		pkTempCol->pkBody1 = pkBody1;
		pkTempCol->pkBody2 = pkBody2;
	
		pkTempCol->fAtime =	fAtime;
		
		m_kCollissions.push_back(pkTempCol);		
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
				
				if(TestLineVSPolygon(verts1,&verts2[i1],&verts2[i2],&P))
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
				}
			}
			
		}
	}
	
	if(bHaveColided)
		return true;
	
	return false;
}

bool Tcs::TestLineVSPolygon(Vector3* pkPolygon,Vector3* pkPos1,Vector3* pkPos2,Plane* pkPlane)
{
//	float fMinDist = 0.1;

	if(pkPlane->LineTest(*pkPos1,*pkPos2,&m_kLastTestPos))
	{
		if(TestSides(pkPolygon,&(pkPlane->m_kNormal),m_kLastTestPos))
		{
			m_kLastTestNormal = -pkPlane->m_kNormal;		
			return true;
			/*
			if( m_kLastTestPos.DistanceTo(*pkPos1) < fMinDist ||
				 m_kLastTestPos.DistanceTo(*pkPos2) < fMinDist ||
				 m_kLastTestPos.DistanceTo(pkPolygon[0]) < fMinDist ||
				 m_kLastTestPos.DistanceTo(pkPolygon[1]) < fMinDist ||
				 m_kLastTestPos.DistanceTo(pkPolygon[2]) < fMinDist)
			{
				//cout<<"Vertex"<<endl;

				m_kLastTestNormal = -pkPlane->m_kNormal;

				return true;
			}
			else
			{
				m_kLastTestNormal = -pkPlane->m_kNormal;
				//m_kLastTestNormal.Set(0,0,0);
				
				//cout<<"EDGE:"<<endl;
				m_kLastTestNormal = -( *pkPos1 - *pkPos2).Cross(pkPolygon[0] - pkPolygon[1]).Unit();
				
				//m_kLastTestNormal = ( *pkPos1 - *pkPos2).Cross(pkPolygon[0] - pkPolygon[1]).Unit();
				//m_kLastTestNormal = -(pkPolygon[0] - pkPolygon[1]).Cross( *pkPos1 - *pkPos2).Unit();
				//m_kLastTestNormal = -(pkPolygon[0] - pkPolygon[1]).Cross( *pkPos1 - *pkPos2).Unit();
			
				return false;
			}
			*/
		}
	}
	
	return false;
}

void Tcs::ResetForces()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
		if(m_kBodys[i]->m_bStatic)
			continue;
				
		m_kBodys[i]->m_kExternalLinearForce.Set(0,0,0);
		m_kBodys[i]->m_kExternalRotForce.Set(0,0,0);	
	
	}
}

void Tcs::ClearCollissions()
{
	for(int i = 0;i<m_kCollissions.size();i++)
	{
		delete m_kCollissions[i];	
	}

	m_kCollissions.clear();
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
