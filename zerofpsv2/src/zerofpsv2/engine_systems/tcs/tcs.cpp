#include "tcs.h"
#include "../propertys/p_tcs.h"
#include "../common/heightmap.h"
	
Tcs::Tcs(): ZFSubSystem("Tcs")
{
	Logf("zerofps","Tiny Collission system created");

 	m_fMaxDelay = 		0.04;
	m_fAlmostZero = 	0.001;
	m_fMinForce = 		0.2;	
	m_fSleepLinVel = 	0.5;
	m_fSleepRotVel = 	0.3;
	m_fMaxVel = 		10.0;
	m_fTimeSlice = 	2.0;
	m_fGravity =		9.84;
	
	m_fSleepTime = 	1.0;
	
	m_iCollisionIterations	= 4;
	m_iContactIterations		= 4;
	
	m_iHandleCollission =1;
	m_iDebugGraph = 		0;
	
	m_iNrOfCollissions = 0;
	m_iNrOfTests = 		0;
	m_iNrOfContacts =		0;
	
	m_pkBodyCopy1 	=	NULL;
	m_pkBodyCopy2	=	NULL;
	
	RegisterVariable("p_tcshandle",		&m_iHandleCollission,CSYS_INT);
	RegisterVariable("p_tcsdebug",		&m_iDebugGraph,CSYS_INT);
	//RegisterVariable("p_tcsdelay",		&m_fMaxDelay,CSYS_FLOAT);
	//RegisterVariable("p_tcsminforce",	&m_fMinForce,CSYS_FLOAT);
	//RegisterVariable("p_tcssleeplinvel",&m_fSleepLinVel,CSYS_FLOAT);
	//RegisterVariable("p_tcssleeprotvel",&m_fSleepRotVel,CSYS_FLOAT);
	RegisterVariable("p_tcsmaxvel",		&m_fMaxVel,CSYS_FLOAT);
	
	RegisterVariable("p_tcssleeptime",	&m_fSleepTime,CSYS_FLOAT);
	RegisterVariable("p_tcscolit",		&m_iCollisionIterations,CSYS_INT);
	RegisterVariable("p_tcsconit",		&m_iContactIterations,CSYS_INT);
}

Tcs::~Tcs()
{
	/*	 blir ju massa evil krasher här så...wtf får la va lite minnes lekage så länge =D
	delete m_pkBodyCopy1;	
	delete m_pkBodyCopy2;
	*/
}


bool Tcs::StartUp()	
{ 
	m_pkZeroFps		= static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));		
	m_pkRender		= static_cast<Render*>(GetSystem().GetObjectPtr("Render"));		
	m_pkEntityMan	= static_cast<EntityManager*>(GetSystem().GetObjectPtr("EntityManager"));		
	
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
	
	StartProfileTimer("s__tcs");				
	
	static float fLastTime= 0; 		
	static float fStepSize = 0.033333;
	
	m_iNrOfCollissions	= 0;
	m_iNrOfTests			= 0;
	m_iNrOfActiveBodies	= 0;	
	m_iNrOfContacts		= 0;

	
	fStepSize = m_pkZeroFps->GetTicks() - fLastTime;
	fLastTime = m_pkZeroFps->GetTicks();
	
	//fStepSize = fAlphaTime;
	
	//minimum time size
	if(fStepSize > 0.05)
		fStepSize = 0.05;
	
	
	
	//synd all bodys to entitys
	SyncBodys();
	
	
	StartProfileTimer("s__tcs-main");
// MAIN LOOP
	
	//calculate current forces
	UpdateForces();		
	
		
	//handle collisions
	for(int i = 0;i< m_iCollisionIterations;i++)
	{
		PushVelPos();
		
		//save old positions and velocitys
		UpdateBodyVels(fStepSize);					
		UpdateBodyPos(fStepSize);
		
		//do collissiontests
		ClearCollissions();				
		UpdateCollissions(&m_kCollissions);
	
		PopVelPos();			

		//handle collission
		m_iNrOfCollissions+=m_kCollissions.size();
		HandleCollissions();
					
	}
	
	m_iNrOfContacts = m_kCollissions.size();
	//cout<<"total collissions:"<<iTotalCollissions<<" contactcs remaining:"<<m_kCollissions.size()<<endl;
		
	//apply forces on velocity
	UpdateBodyVels(fStepSize);		

	
	//do shock test
	Shock();
	
	//handle contacts
	for(int i = 0;i< m_iContactIterations;i++)
	{
		PushVelPos();
		
		//save old positions and velocitys
		UpdateBodyPos(fStepSize);
		
		//do collissiontests
		ClearCollissions();				
		UpdateCollissions(&m_kCollissions);
		
		PopVelPos();			

		//handle collission
		StopObjects(fStepSize);
	}
	

	//now move all objects	
	UpdateBodyPos(fStepSize);
//---------------- End of maaaaaaaaaain loop	
	
	StopProfileTimer("s__tcs-main");
	
	//update all character ground line tests	
	UpdateLineTests(fStepSize);


	
	//check for resting bodys
	static float fLastRestFind = 0;	
	if(m_pkZeroFps->GetTicks() - fLastRestFind > m_fSleepTime)
	{
		fLastRestFind = m_pkZeroFps->GetTicks();
		FindRestingBodys();
	}
		
	//synd all entitys to bodys
	SyncEntitys();
	
	//clear all forces
	ResetForces();
	
	
	StopProfileTimer("s__tcs");			
}

void Tcs::PushVelPos()
{
	m_kOldPos.clear();
	m_kOldRot.clear();
	m_kOldVel.clear();
	m_kOldRotVel.clear();


	for(unsigned int i=0;i<m_kBodys.size();i++)
	{
		m_kOldPos.push_back(m_kBodys[i]->m_kNewPos);
		m_kOldVel.push_back(m_kBodys[i]->m_kLinearVelocity);
		
		m_kOldRot.push_back(m_kBodys[i]->m_kNewRotation);
		m_kOldRotVel.push_back(m_kBodys[i]->m_kRotVelocity);
	}
}

void Tcs::PopVelPos()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{
		m_kBodys[i]->m_kNewPos = m_kOldPos[i];
		m_kBodys[i]->m_kLinearVelocity = m_kOldVel[i];
		
		m_kBodys[i]->m_kNewRotation = m_kOldRot[i];
		m_kBodys[i]->m_kRotVelocity = m_kOldRotVel[i];
	}
}

void Tcs::ClearCollissions()
{
	for(int i = 0 ;i < m_kCollissions.size();i++)
		delete m_kCollissions[i];

	m_kCollissions.clear();
}

void Tcs::HandleCollissions()
{
	for(int i = 0 ;i<m_kCollissions.size();i++)
	{
		HandleCollission(m_kCollissions[i]);
	}
}

void Tcs::ResetTempStatics()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{				
		m_kBodys[i]->m_bTempStatic = false;
	}
}

void Tcs::Shock()
{
	for(int j = 0;j<5;j++)
	{	
		for(int i = 0;i<m_kCollissions.size();i++)
		{
			//check if both are now static
			if(	(m_kCollissions[i]->pkBody1->m_bStatic || m_kCollissions[i]->pkBody1->m_bTempStatic) && 
					(m_kCollissions[i]->pkBody2->m_bStatic || m_kCollissions[i]->pkBody2->m_bTempStatic)	)
					continue;
		
			//is one static?
			if(	m_kCollissions[i]->pkBody1->m_bStatic || m_kCollissions[i]->pkBody1->m_bTempStatic
				||	m_kCollissions[i]->pkBody2->m_bStatic || m_kCollissions[i]->pkBody2->m_bTempStatic)
			{
				HandleCollission(m_kCollissions[i],true,false);
				
				if(m_kCollissions[i]->pkBody1->m_bStatic || m_kCollissions[i]->pkBody1->m_bTempStatic)
					m_kCollissions[i]->pkBody2->m_bTempStatic = true;
				else
					m_kCollissions[i]->pkBody1->m_bTempStatic = true;
			
			}
		}			
	}

	//clear temp statics
	ResetTempStatics();	
}

void Tcs::StopObjects(float fAtime)
{
	for(int i = 0 ;i<m_kCollissions.size();i++)
	{
		//handle collision with no bounce
		HandleCollission(m_kCollissions[i],true);
	}
}

void Tcs::UpdateLineTests(float fAlphaTime) 
{
	float distance;

	for(unsigned int i=0;i<m_kBodys.size();i++)
	{
		if(m_kBodys[i]->m_bStatic || m_kBodys[i]->m_bSleeping)
			continue;
					
		if(m_kBodys[i]->m_bCharacter)
		{					
			bool bTuchedGround = false;
			 
			//if(TestLine(m_kBodys[i]->m_kNewPos,Vector3(0,-1,0),m_kBodys[i]))
			if(CharacterLineTest(m_kBodys[i]->m_kNewPos,Vector3(0,-1,0),m_kBodys[i]))
			{		
				//make sure thers no x/z movement (some tests are not exact
				m_kLastLineTestColPos.x = m_kBodys[i]->m_kNewPos.x;
				m_kLastLineTestColPos.z = m_kBodys[i]->m_kNewPos.z;
				
				distance = m_kBodys[i]->m_kNewPos.DistanceTo(m_kLastLineTestColPos);
				if(distance < m_kBodys[i]->m_fLegLength)
				{
					bTuchedGround = true;
				
					m_kBodys[i]->m_kNewPos = m_kLastLineTestColPos + Vector3(0,m_kBodys[i]->m_fLegLength*0.9,0);
					m_kBodys[i]->m_bOnGround = true;
					m_kBodys[i]->m_kLinearVelocity.y= 0;
				
					//ground breaks
					m_kBodys[i]->m_kLinearVelocity -= 10*(m_kBodys[i]->m_kLinearVelocity * fAlphaTime);											
				}
			}
			
			//what do do if we didt touch the ground?			
			if(!bTuchedGround)
			{
				m_kBodys[i]->m_bOnGround = false;			
				

				//add breaks to character :D					
				float fOld = m_kBodys[i]->m_kLinearVelocity.y;
			
				//air breaks
				m_kBodys[i]->m_kLinearVelocity -= 4*(m_kBodys[i]->m_kLinearVelocity * fAlphaTime);						
			
				//m_kBodys[i]->m_kLinearVelocity.Set(0,fOld,0);
				m_kBodys[i]->m_kLinearVelocity.y = fOld;
			}		
		}
	}
}

void Tcs::HandleCollission(Tcs_collission* pkCol,bool bNoBounce,bool bNoAngular)
{	
	//wake up bodys
	pkCol->pkBody1->Wakeup();
	pkCol->pkBody2->Wakeup();

	// masses
	float fMass1 = pkCol->pkBody1->m_fMass;
	float fMass2 = pkCol->pkBody2->m_fMass;
	
	//treat static bodys as having infinit mass	
	if(pkCol->pkBody1->m_bStatic)
	{
		fMass1 = 999999999;	
	}
	
	if(pkCol->pkBody2->m_bStatic)
	{
		fMass2 = 999999999;	
	}
		
		
	//bounce	
	float fBounce = pkCol->pkBody1->m_fBounce * pkCol->pkBody2->m_fBounce;
	
	if(bNoBounce)
		fBounce = 0.1;
	
	//friction
	float fFriction = pkCol->pkBody1->m_fFriction * pkCol->pkBody2->m_fFriction;
	
	//setup vels	
	Vector3 kLinearRelVel = pkCol->pkBody1->m_kLinearVelocity - pkCol->pkBody2->m_kLinearVelocity;
	Vector3 kRotVel1 = pkCol->pkBody1->m_kRotVelocity;
	Vector3 kRotVel2 = pkCol->pkBody2->m_kRotVelocity;

	//nr of collision points
	int iNrOfPos = pkCol->kPositions.size();
	
			
	//handle all collision points
	for(int i = 0;i<	iNrOfPos;i++)
	{	
		if(m_iDebugGraph)
			m_pkRender->Line(pkCol->kPositions[i],pkCol->kPositions[i] + pkCol->kNormals[i]);		
		
		//collission handling is disabled
		if(!m_iHandleCollission)
			continue;			
			
		//calculate the relative velocity betwen collission points
		Vector3 kRelVel = kLinearRelVel + (-kRotVel1.Cross(pkCol->kPositions[i] - pkCol->pkBody1->m_kNewPos) - -kRotVel2.Cross(pkCol->kPositions[i] - pkCol->pkBody2->m_kNewPos));						
		
		//check relative velocity, Skip if collission points are moving away from each other
		if(pkCol->kNormals[i].Dot(kRelVel) >= 0)
			continue;						
					
		//calculate collission tangent
		Vector3 kTangent = (pkCol->kNormals[i].Cross(kRelVel.Unit())).Cross(pkCol->kNormals[i]);
					
		//calculcate collision impact force				
		float j  =	(-(1.0+fBounce) * (kRelVel * pkCol->kNormals[i])) /
						( (pkCol->kNormals[i] * pkCol->kNormals[i]) *
						( 1.0/fMass1 + 1.0/fMass2)); 			
					
		//divide force with nr of impact points
		j /= float(iNrOfPos);
		
				
		//check if already solved
		int iLoops = 0;
		while(	pkCol->kNormals[i].Dot(
				( (pkCol->pkBody1->m_kLinearVelocity-pkCol->pkBody2->m_kLinearVelocity) + 
				(-pkCol->pkBody1->m_kRotVelocity.Cross(pkCol->kPositions[i] - pkCol->pkBody1->m_kNewPos) - 
				-pkCol->pkBody2->m_kRotVelocity.Cross(pkCol->kPositions[i] - pkCol->pkBody2->m_kNewPos)) )) <= 0	)
		{					
			iLoops++;			
			if(iLoops >= 4)			
				break;
		
			// APPLY IMPULSES					
			if( (!pkCol->pkBody1->m_bStatic) && (!pkCol->pkBody1->m_bTempStatic) )
			{
				if(bNoAngular)
				{
					pkCol->pkBody1->ApplyImpulsForce(pkCol->kNormals[i] * j );							
					pkCol->pkBody1->ApplyImpulsForce(-kTangent * (j * fFriction) );
				}
				else
				{
					pkCol->pkBody1->ApplyImpulsForce(pkCol->kPositions[i],pkCol->kNormals[i] * j ,false);							
					pkCol->pkBody1->ApplyImpulsForce(pkCol->kPositions[i],-kTangent * (j * fFriction) ,false);
				}
			}		
			
			if( (!pkCol->pkBody2->m_bStatic) && (!pkCol->pkBody2->m_bTempStatic) )
			{
				if(bNoAngular)
				{
					pkCol->pkBody2->ApplyImpulsForce(-pkCol->kNormals[i] * j );							
					pkCol->pkBody2->ApplyImpulsForce(kTangent * (j * fFriction) );				
				}
				else
				{
					pkCol->pkBody2->ApplyImpulsForce(pkCol->kPositions[i],-pkCol->kNormals[i] * j ,false);							
					pkCol->pkBody2->ApplyImpulsForce(pkCol->kPositions[i],kTangent * (j * fFriction) ,false);
				}
			}						
		}
	}

	//sync entitys before touching	
/*	SyncEntitys();
	//touch objects	
	pkCol->pkBody1->GetEntity()->Touch(pkCol->pkBody2->GetEntity()->GetEntityID());
	pkCol->pkBody2->GetEntity()->Touch(pkCol->pkBody1->GetEntity()->GetEntityID());	
	//sync bodys after touching
	SyncBodys();
*/	
	//try to put bodys to sleep
	//TryToSleep(pkCol->pkBody1,pkCol->pkBody2);						
}

void Tcs::FindRestingBodys()
{	
	static float fRestMaxDist = 0.05;

	for(int i = 0;i<m_kBodys.size();i++)
	{
		if(m_kBodys[i]->m_bStatic ||  m_kBodys[i]->m_bSleeping)
			continue;
	
		if(m_kBodys[i]->m_kNewPos.DistanceTo(m_kBodys[i]->m_kLastPos) <= fRestMaxDist)
		{
			m_kBodys[i]->m_kLastPos = m_kBodys[i]->m_kNewPos;
		
			m_kBodys[i]->Sleep();
			
			if(m_kBodys[i]->m_bDisableOnSleep)
			{
				m_kBodys[i]->Disable();
				m_kBodys[i]->GetEntity()->DeleteProperty("P_Tcs");;
			}
		}
			
		//update last pos			
		m_kBodys[i]->m_kLastPos = m_kBodys[i]->m_kNewPos;
	}
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
		
		m_kBodys[i]->GetEntity()->SetWorldPosV(m_kBodys[i]->m_kNewPos);
		m_kBodys[i]->GetEntity()->SetVel(m_kBodys[i]->m_kLinearVelocity);
		m_kBodys[i]->GetEntity()->SetLocalRotM(m_kBodys[i]->m_kNewRotation);
	}
}

void Tcs::SyncBodys()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{
		m_kBodys[i]->m_kNewPos = m_kBodys[i]->GetEntity()->GetWorldPosV();		
		m_kBodys[i]->m_kNewRotation = m_kBodys[i]->GetEntity()->GetLocalRotM();
		//m_kBodys[i]->m_kLinearVelocity = m_kBodys[i]->GetObject()->GetVel();							
	}
}

void Tcs::UpdateBodyVels(float fAtime)
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
		UpdateBodyVel(m_kBodys[i],fAtime);					
	}
}

void Tcs::UpdateBodyVel(P_Tcs* pkBody,float fAtime)
{
	if(pkBody->m_bStatic || pkBody->m_bSleeping || pkBody->m_bTempStatic)
		return;
	
								
	//apply linear acceleration
	pkBody->m_kLinearVelocity += pkBody->m_kLinearForce * fAtime;	
	//apply rotation acceleration
	pkBody->m_kRotVelocity += pkBody->m_kRotForce * fAtime;

	
	//check max velocitys
	if(pkBody->m_kLinearVelocity.Length() > m_fMaxVel)
		pkBody->m_kLinearVelocity = pkBody->m_kLinearVelocity.Unit() * m_fMaxVel;

}


void Tcs::UpdateBodyPos(float fAtime)
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
		if(m_kBodys[i]->m_bStatic || m_kBodys[i]->m_bSleeping || m_kBodys[i]->m_bTempStatic)
			continue;

			
		//Calculate new position
		m_kBodys[i]->m_kNewPos += (m_kBodys[i]->m_kLinearVelocity * fAtime);
		//calculate new rotation 
		m_kBodys[i]->m_kNewRotation.RadRotate(m_kBodys[i]->m_kRotVelocity * fAtime);					
	}
}


void Tcs::UpdateForces()
{
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{	
		if(m_kBodys[i]->m_bStatic || m_kBodys[i]->m_bSleeping || m_kBodys[i]->m_bTempStatic)
			continue;

		//increse active body count
		m_iNrOfActiveBodies++;
			
				
		// LINEAR FORCE / ACCLERERATION			
			m_kBodys[i]->m_kLinearForce.Set(0,0,0);
		
			//add external linear force
			m_kBodys[i]->m_kLinearForce += m_kBodys[i]->m_kExternalLinearForce; 
					
			//apply gravity if enabled
			if(m_kBodys[i]->m_bGravity)
				m_kBodys[i]->m_kLinearForce.y -= m_fGravity * m_kBodys[i]->m_fMass;

			Vector3 kY;
			m_kBodys[i]->GetVel(kY);
			
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

void Tcs::UpdateCollissions(vector<Tcs_collission*>*	pkCollissions)
{
	//first update all bounding boxes
	UpdateAABBs();

	//loop trough all bodys and testing for collisions against each other
	for(unsigned int B1=0;B1<m_kBodys.size();B1++)
	{
		for(unsigned int B2=B1+1;B2<m_kBodys.size();B2++)
		{					
			//dont test if both bodys are static or sleeping
			if( (m_kBodys[B1]->m_bStatic || m_kBodys[B1]->m_bSleeping) && (m_kBodys[B2]->m_bStatic || m_kBodys[B2]->m_bSleeping))
				continue;		
				
			//check collission groups
			if( !(m_kBodys[B1]->m_akTestGroups[m_kBodys[B2]->m_iGroup] ||
				m_kBodys[B2]->m_akTestGroups[m_kBodys[B1]->m_iGroup]) )			
				continue;
				
			//check if entitys are in neighbour zone
			if(!IsInNerbyZone(m_kBodys[B1],m_kBodys[B2]))
				continue;				
								
			//first do a sphere test			
			if(!CollideSphereVSSphere(m_kBodys[B1],m_kBodys[B2]))
				continue;
				
			if(!TestAABB(m_kBodys[B1],m_kBodys[B2]))
				continue;
			
			//increse number of tests
			m_iNrOfTests++;

												
			if(((m_kBodys[B1]->m_iTestType==E_BOX) && (m_kBodys[B2]->m_iTestType==E_MESH)) || 
				((m_kBodys[B1]->m_iTestType==E_MESH) && (m_kBodys[B2]->m_iTestType==E_BOX)))
			{
				//BOX VS MESH
				TestBoxVsMesh(m_kBodys[B1],m_kBodys[B2],pkCollissions);
			}
			else if( (m_kBodys[B1]->m_iTestType==E_BOX) && (m_kBodys[B2]->m_iTestType==E_BOX) )
			{
				//BOX VS BOX
				TestBoxVsBox(m_kBodys[B1],m_kBodys[B2],pkCollissions);
			}
			else if( (m_kBodys[B1]->m_iTestType==E_MESH) && (m_kBodys[B2]->m_iTestType==E_MESH) )
			{
				//MESH VS MESH
				TestMeshVsMesh(m_kBodys[B1],m_kBodys[B2],pkCollissions);
			}
			
			else if( (m_kBodys[B1]->m_iTestType==E_MESH) || (m_kBodys[B2]->m_iTestType==E_MESH))
			{				
				//SPHERE VS MESH
				TestSphereVsMesh(m_kBodys[B1],m_kBodys[B2],pkCollissions);
			}
			else if( (m_kBodys[B1]->m_iTestType==E_SPHERE) && (m_kBodys[B2]->m_iTestType==E_SPHERE) ) 
			{
				//SPHERE VS SPHERE
				TestSphereVsSphere(m_kBodys[B1],m_kBodys[B2],pkCollissions);
			}
			else if	(((m_kBodys[B1]->m_iTestType==E_SPHERE) && (m_kBodys[B2]->m_iTestType==E_HMAP)) || 
						((m_kBodys[B1]->m_iTestType==E_HMAP) && (m_kBodys[B2]->m_iTestType==E_SPHERE)))
			{
				//SPHERE VS MESH ( HEIGHTMAP)
				TestSphereVsMesh(m_kBodys[B1],m_kBodys[B2],pkCollissions);			
			}
		}
	}
}


void Tcs::HandleCharacterCollission(P_Tcs* pkCharacter,P_Tcs* pkBody)
{
	if(pkCharacter->m_akWalkableGroups[pkBody->m_iGroup])
	{
			
		Vector3 dir = (m_kLastTestPos - pkCharacter->GetEntity()->GetWorldPosV()).Unit();
		
		pkCharacter->m_kNewPos = pkCharacter->GetEntity()->GetWorldPosV() - dir * 0.1;
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

void Tcs::UpdateAABBs()
{
	static Vector3 kSize;
	static Vector3 kPos[8];
	static Vector3 kVertex;
	

	for(int i = 0;i<m_kBodys.size();i++)
	{
		P_Tcs* pkBody = m_kBodys[i];
	
		//if body is static or sleeping check if rotation has changes, if not dont update
		if(pkBody->m_bStatic || pkBody->m_bSleeping)
		{
			//unchanged rotation
			if(pkBody->m_kAABBRotation == pkBody->m_kNewRotation)			
				continue;
			
			//save new aabb rotation	
			pkBody->m_kAABBRotation = pkBody->m_kNewRotation;				
		}
		
		switch(pkBody->m_iTestType)
		{
			case E_BOX:				
			{
				kSize = pkBody->m_kBoxSize*0.5;
			
				kPos[0] = pkBody->m_kNewRotation.VectorTransform(kSize);
				kPos[1] = pkBody->m_kNewRotation.VectorTransform(Vector3(kSize.x,-kSize.y,kSize.z));
				kPos[2] = pkBody->m_kNewRotation.VectorTransform(Vector3(-kSize.x,kSize.y,kSize.z));
				kPos[3] = pkBody->m_kNewRotation.VectorTransform(Vector3(-kSize.x,-kSize.y,kSize.z));
				kPos[4] = pkBody->m_kNewRotation.VectorTransform(Vector3(kSize.x,kSize.y,-kSize.z));
				kPos[5] = pkBody->m_kNewRotation.VectorTransform(Vector3(kSize.x,-kSize.y,-kSize.z));
				kPos[6] = pkBody->m_kNewRotation.VectorTransform(Vector3(-kSize.x,kSize.y,-kSize.z));
				kPos[7] = pkBody->m_kNewRotation.VectorTransform(Vector3(-kSize.x,-kSize.y,-kSize.z));
				
				pkBody->m_kAABBMax = kPos[0];
				pkBody->m_kAABBMin = kPos[0];
				
				for(int j = 0;j<8;j++)
				{
					//max
					if(kPos[j].x > pkBody->m_kAABBMax.x)
						pkBody->m_kAABBMax.x = kPos[j].x;

					if(kPos[j].y > pkBody->m_kAABBMax.y)
						pkBody->m_kAABBMax.y = kPos[j].y;
					
					if(kPos[j].z > pkBody->m_kAABBMax.z)
						pkBody->m_kAABBMax.z = kPos[j].z;
						
					//min	
					if(kPos[j].x < pkBody->m_kAABBMin.x)
						pkBody->m_kAABBMin.x = kPos[j].x;

					if(kPos[j].y < pkBody->m_kAABBMin.y)
						pkBody->m_kAABBMin.y = kPos[j].y;
					
					if(kPos[j].z < pkBody->m_kAABBMin.z)
						pkBody->m_kAABBMin.z = kPos[j].z;																	
				}
								
				break;
			}
			
			case E_SPHERE:
				pkBody->m_kAABBMax.Set(pkBody->m_fRadius,pkBody->m_fRadius,pkBody->m_fRadius);
				pkBody->m_kAABBMin.Set(-pkBody->m_fRadius,-pkBody->m_fRadius,-pkBody->m_fRadius);

				break;
			
			// mesh and hmap uses the same
			case E_HMAP:
			case E_MESH:
			{
				if(!pkBody->m_bHavePolygonData)
				{
					pkBody->m_kAABBMax.Set(pkBody->m_fRadius,pkBody->m_fRadius,pkBody->m_fRadius);
					pkBody->m_kAABBMin.Set(-pkBody->m_fRadius,-pkBody->m_fRadius,-pkBody->m_fRadius);		
					break;
				}				
				
				pkBody->m_kAABBMax.Set(-9999999,-9999999,-9999999);	
				pkBody->m_kAABBMin.Set(9999999,9999999,9999999);	
				
				for(unsigned int f=0;f<pkBody->m_pkVertex->size();f++)
				{
					kVertex = pkBody->m_kNewRotation.VectorTransform((*pkBody->m_pkVertex)[f]);
					kVertex*= pkBody->m_fScale;
					
					//max
					if(kVertex.x > pkBody->m_kAABBMax.x)
						pkBody->m_kAABBMax.x = kVertex.x;

					if(kVertex.y > pkBody->m_kAABBMax.y)
						pkBody->m_kAABBMax.y = kVertex.y;
					
					if(kVertex.z > pkBody->m_kAABBMax.z)
						pkBody->m_kAABBMax.z = kVertex.z;	
						
					//min	
					if(kVertex.x < pkBody->m_kAABBMin.x)
						pkBody->m_kAABBMin.x = kVertex.x;

					if(kVertex.y < pkBody->m_kAABBMin.y)
						pkBody->m_kAABBMin.y = kVertex.y;
					
					if(kVertex.z < pkBody->m_kAABBMin.z)
						pkBody->m_kAABBMin.z = kVertex.z;							
				}
				
				break;
			}
		}		
	}
}

bool Tcs::TestAABB(P_Tcs* pkBody1,P_Tcs* pkBody2)
{
	
	if( (pkBody2->m_kNewPos.x + pkBody2->m_kAABBMin.x ) > (pkBody1->m_kNewPos.x + pkBody1->m_kAABBMax.x ) )
		return false;
		
	if( (pkBody2->m_kNewPos.x + pkBody2->m_kAABBMax.x ) < (pkBody1->m_kNewPos.x + pkBody1->m_kAABBMin.x ) )
		return false;
		
	if( (pkBody2->m_kNewPos.z + pkBody2->m_kAABBMin.z ) > (pkBody1->m_kNewPos.z + pkBody1->m_kAABBMax.z ) )
		return false;

	if( (pkBody2->m_kNewPos.z + pkBody2->m_kAABBMax.z ) < (pkBody1->m_kNewPos.z + pkBody1->m_kAABBMin.z ) )
		return false;

	if( (pkBody2->m_kNewPos.y + pkBody2->m_kAABBMin.y ) > (pkBody1->m_kNewPos.y + pkBody1->m_kAABBMax.y ) )
		return false;	
		
	if( (pkBody2->m_kNewPos.y + pkBody2->m_kAABBMax.y ) < (pkBody1->m_kNewPos.y + pkBody1->m_kAABBMin.y ) )
		return false;


	return true;				
}

bool Tcs::IsInNerbyZone(P_Tcs* pkBody1,P_Tcs* pkBody2)
{	
	int iZoneID1 = pkBody1->GetEntity()->GetCurrentZone();
	int iZoneID2 = pkBody2->GetEntity()->GetCurrentZone();
		
	
	if(iZoneID1 == iZoneID2)
		return true;
	
	if(ZoneData* pkZoneData = m_pkEntityMan->GetZoneData(iZoneID1))
	{
		for(int i = 0;i<pkZoneData->m_iZoneLinks.size();i++)
		{
			if(pkZoneData->m_iZoneLinks[i] == iZoneID2)
				return true;
		}
	}
	
	return false;
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

P_Tcs* Tcs::CharacterLineTest(Vector3 kStart,Vector3 kDir,P_Tcs* pkTester)
{
	m_kLastLineTestColPos =	kStart;		
	float fClosest = 			999999999;
	P_Tcs* pkClosest =		NULL;	
	float d;
	
	Vector3 kPos2 = kStart + kDir*10;
		
	for(unsigned int i=0;i<m_kBodys.size();i++)
	{				
		if(	(m_kBodys[i] == pkTester) || 
				(!pkTester->m_akWalkableGroups[m_kBodys[i]->m_iGroup]) )
			continue;
			
	
		//check if entitys are in neighbour zone
		if(!IsInNerbyZone(pkTester,m_kBodys[i]))
			continue;							
		
			
		switch(m_kBodys[i]->m_iTestType)
		{
			case E_MESH:
				if(CharacterTestLineVSSphere(kStart,kPos2,m_kBodys[i]))
					if(CharacterTestLineVSMesh(kStart,kDir,m_kBodys[i]))
					{
						d = kStart.DistanceTo(m_kLastTestPos);				
						if(d < fClosest)
						{						
							m_kLastLineTestColPos = m_kLastTestPos;
							fClosest = d;
							pkClosest = m_kBodys[i];
						}													
					}					
				break;
				
			case E_HMAP:
				if(CharacterTestLineVSSphere(kStart,kPos2,m_kBodys[i]))
					if(CharacterTestLineVSMesh(kStart,kDir,m_kBodys[i]))
					{
						d = kStart.DistanceTo(m_kLastTestPos);				
						if(d < fClosest)
						{						
							m_kLastLineTestColPos = m_kLastTestPos;
							fClosest = d;
							pkClosest = m_kBodys[i];
						}													
					}					
				break;
		}
	}

	return pkClosest;
}

P_Tcs* Tcs::TestLine(Vector3 kStart,Vector3 kDir,P_Tcs* pkTester)
{
	m_kLastLineTestColPos = kStart;		
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
			d = kStart.DistanceTo(m_kLastTestPos);				
			if(d < closest)
			{
				
				m_kLastLineTestColPos = m_kLastTestPos;
				closest = d;
				pkClosest = m_kBodys[i];
			}				
		}		
	}		
	
	return pkClosest;
}

bool Tcs::CharacterTestLineVSMesh(Vector3 kStart,Vector3 kDir,P_Tcs* pkMesh)
{
	Matrix4 kModelMatrix = pkMesh->GetModelMatrix();

	
	float closest = 		99999999;
	bool bHaveColided = 	false;	
	Vector3 kClosestNormal;
	Vector3 kClosestPos;
	
	Vector3 kPoint2 = kStart + kDir * 1000;
	Vector3 verts[3];	
	float d;
	
	for(unsigned int f=0;f<pkMesh->m_pkFaces->size();f++)
	{		 
		verts[0] = kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[0]]);
		verts[1] = kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[1]]);		
		verts[2] = kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[2]]);		
		
		/*
		if(m_iDebugGraph == 2)
		{
			//debug stuff
			m_pkRender->Line(verts[0],verts[1]);
			m_pkRender->Line(verts[1],verts[2]);		
			m_pkRender->Line(verts[2],verts[0]);				
		}	*/	
		
		if(TestLineVSPolygon(verts,&kStart,&kPoint2))
		{	
			d = kStart.DistanceTo(m_kLastTestPos);
		
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

bool Tcs::TestLineVSPolygon(Vector3* pkVerts,Vector3* pkPos1,Vector3* pkPos2)
{
	static Plane P;
	static Vector3 V1,V2,Normal;

	V1 = pkVerts[1] - pkVerts[0];
	V2 = pkVerts[2] - pkVerts[0];		
	Normal= V1.Cross(V2);
	
	if(Normal.Length() == 0)
		return false;
	
	Normal.Normalize();
	P.m_fD = -Normal.Dot(pkVerts[0]);	
	P.m_kNormal = Normal;

	if(P.LineTest(*pkPos1 , *pkPos2 ,&m_kLastTestPos))
		if(TestSides(pkVerts,&Normal,m_kLastTestPos))
			return true;
	
	return false;	
}

bool Tcs::TestLineVSMesh(Vector3 kStart,Vector3 kDir,P_Tcs* pkB)
{
	if(pkB->m_pkMad)
	{	
		if(pkB->m_pkMad->TestLine(kStart,kDir))
		{
			m_kLastTestPos = pkB->m_pkMad->GetLastColPos();
			return true;
		}
	}
	else if (pkB->m_pkHmap) 
	{
		if(pkB->LineVSMesh(kStart,kDir))
		{
			m_kLastTestPos = pkB->m_kColPos;
			return true;
		}
	}

	return false;	
}

bool Tcs::CharacterTestLineVSSphere(Vector3 kP1,Vector3 kP2,P_Tcs* pkB)
{
	static Vector3 kDir;
	static Vector3 c;
	static Vector3 k;
	
	kDir = kP2 - kP1;
	c=pkB->m_kNewPos - kP1;		

	kDir.Normalize();		
	
	float d = kDir.Dot(c);
	k=kDir.Proj(c);		
	float cdis=c.Length();
	float kdis=k.Length();
	float Distance = (float) sqrt((cdis*cdis)-(kdis*kdis));
	

	if(Distance < pkB->m_fRadius)
	{			
		m_kLastTestPos = pkB->m_kNewPos - c.Unit() * pkB->m_fRadius; //kP1 + k;
		//m_kLastTestPos.x = kP1.x;
		//m_kLastTestPos.z = kP1.z;
		//m_kLastTestPos = kP1 + k;	
		return true;
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
		//m_kLastTestPos = pkB->m_kNewPos - c.Unit() * pkB->m_fRadius; //kP1 + k;
		//m_kLastTestPos.x = kP1.x;
		//m_kLastTestPos.z = kP1.z;
		m_kLastTestPos = kP1 + k;
		
		return true;
	}		
	
	return false;
}


void Tcs::TestSphereVsSphere(P_Tcs* pkBody1,P_Tcs* pkBody2,vector<Tcs_collission*>*	pkCollissions)
{
	if(CollideSphereVSSphere(pkBody1,pkBody2))
	{
		Tcs_collission* temp = new Tcs_collission;
		temp->Clear();
		
		temp->pkBody1 = pkBody1;
		temp->pkBody2 = pkBody2;
	
		temp->kNormals.push_back((pkBody1->m_kNewPos - pkBody2->m_kNewPos).Unit());
		temp->kPositions.push_back(pkBody1->m_kNewPos - (temp->kNormals[0] * pkBody1->m_fRadius));
					
		
		pkCollissions->push_back(temp);							
		
	}		
		
/*		
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
*
			//Tcs_collission* temp = new Tcs_collission;
			pkCollission->Clear();
			
			pkCollission->pkBody1 = pkBody1;
			pkCollission->pkBody2 = pkBody2;
		
			pkCollission->kNormals.push_back((m_pkBodyCopy1->m_kNewPos - m_pkBodyCopy2->m_kNewPos).Unit());
			pkCollission->kPositions.push_back(m_pkBodyCopy1->m_kNewPos - (pkCollission->kNormals[0] * m_pkBodyCopy1->m_fRadius));
			pkCollission->fAtime =	fAtime;
			
			//m_kCollissions.push_back(temp);					
		}
	}*/
}

bool Tcs::CollideSphereVSSphere(P_Tcs* pkBody1,P_Tcs* pkBody2)
{
	return (	pkBody1->m_kNewPos.DistanceTo(pkBody2->m_kNewPos) - 
				(pkBody1->m_fRadius + pkBody2->m_fRadius) ) <= 0;
}


void Tcs::TestSphereVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,vector<Tcs_collission*>*	pkCollissions)
{
	bool retry = true;
	bool didpen = false;
	int nroftests = 0;
	
	//if body1 is the mesh just flip them,body2 shuld always be the mesh
	if( (pkBody1->m_iTestType==E_MESH) || pkBody1->m_iTestType==E_HMAP )
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
	
	if(CollideSphereVSMesh(pkBody1,pkBody2))
	{
		Tcs_collission* temp = new Tcs_collission;
		
		temp->Clear();
		
		temp->pkBody1 = pkBody1;
		temp->pkBody2 = pkBody2;	
		temp->kNormals.push_back((pkBody1->m_kNewPos - m_kLastTestPos).Unit());
		temp->kPositions.push_back(m_kLastTestPos);
					
		pkCollissions->push_back(temp);				
	}
		
/*	while(retry && (fAtime >0.0) )
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
	}	*/
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

void Tcs::TestBoxVsBox(P_Tcs* pkBody1,P_Tcs* pkBody2,vector<Tcs_collission*>*	pkCollissions)
{
	Tcs_collission* temp = new Tcs_collission;
	temp->Clear();
	
	if(CollideBoxVSBox(pkBody1,pkBody2,temp))
	{		
		temp->pkBody1 = pkBody1;
		temp->pkBody2 = pkBody2;		
		
		pkCollissions->push_back(temp);				
		
	}	
	else
		delete temp;	

}

void Tcs::TestBoxVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,vector<Tcs_collission*>*	pkCollissions)
{
	//flip bodys body1 is always the box and body2 is always the mesh
	if(pkBody1->m_iTestType == E_MESH)
	{
		P_Tcs* b2c = pkBody2;	
		pkBody2 = pkBody1;
		pkBody1 = b2c;	
	}

	//cout<<"hippo"<<endl;
	
	Tcs_collission* temp = new Tcs_collission;
	temp->Clear();
	
	if(CollideBoxVSMesh(pkBody1,pkBody2,temp))
	{		
		temp->pkBody1 = pkBody1;
		temp->pkBody2 = pkBody2;		
		
		pkCollissions->push_back(temp);				
		
	}	
	else
		delete temp;
}

void Tcs::TestMeshVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,vector<Tcs_collission*>*	pkCollissions)
{
	//make sure body 1 and 2 have all the pointers else get angry and cry
	if( !pkBody2->m_pkVertex || !pkBody2->m_pkFaces || !pkBody1->m_pkVertex || !pkBody1->m_pkFaces)
		return;		

/*			
	bool retry = 			true;
	bool didpen = 			false;
	int nroftests = 		1;
	float fLastColTime = fAtime;
	float fCT = 			0;
	float	fLastNoColTime = 0;
	float fStep = 			fAtime / 8.0;
	
	
	

	memcpy(m_pkBodyCopy1,pkBody1,sizeof(P_Tcs));
	memcpy(m_pkBodyCopy2,pkBody2,sizeof(P_Tcs));						
	UpdateBodyVelnPos(m_pkBodyCopy1,fAtime);
	UpdateBodyVelnPos(m_pkBodyCopy2,fAtime);	
*/	
	//m_iNrOfTests++;	
	if(CollideMeshVSMesh3(pkBody1,pkBody2,NULL))
	{

		Tcs_collission* temp = new Tcs_collission;
		temp->Clear();
		
		if(CollideMeshVSMesh3(pkBody1,pkBody2,temp))
		{		
			temp->pkBody1 = pkBody1;
			temp->pkBody2 = pkBody2;
			
			
			pkCollissions->push_back(temp);				
		}	
		else
			delete temp;
	}

/*		
	//first do a check att max time
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
			if(CollideMeshVSMesh3(m_pkBodyCopy1,m_pkBodyCopy2,pkCollission))
			{
				cout<<"FUCKING DAM SHIT SOMEHING IS DAAAMN WRONG HERE"<<endl;
				return;
			}
		
			pkCollission->pkBody1 = pkBody1;
			pkCollission->pkBody2 = pkBody2;		
			pkCollission->fAtime =	fLastNoColTime;
		}
	}	
*/	
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

bool Tcs::TestPolygonAABB(Vector3* pkPolygon1,Vector3* pkPolygon2)
{
	static Vector3 kMin1,kMin2,kMax1,kMax2;
	
	kMin1.x = Min(Min(pkPolygon1[0].x,pkPolygon1[1].x),pkPolygon1[2].x);
	kMin1.y = Min(Min(pkPolygon1[0].y,pkPolygon1[1].y),pkPolygon1[2].y);
	kMin1.z = Min(Min(pkPolygon1[0].z,pkPolygon1[1].z),pkPolygon1[2].z);
	
	kMin2.x = Min(Min(pkPolygon2[0].x,pkPolygon2[1].x),pkPolygon2[2].x);
	kMin2.y = Min(Min(pkPolygon2[0].y,pkPolygon2[1].y),pkPolygon2[2].y);
	kMin2.z = Min(Min(pkPolygon2[0].z,pkPolygon2[1].z),pkPolygon2[2].z);
	
	kMax1.x = Max(Max(pkPolygon1[0].x,pkPolygon1[1].x),pkPolygon1[2].x);
	kMax1.y = Max(Max(pkPolygon1[0].y,pkPolygon1[1].y),pkPolygon1[2].y);
	kMax1.z = Max(Max(pkPolygon1[0].z,pkPolygon1[1].z),pkPolygon1[2].z);

	kMax2.x = Max(Max(pkPolygon2[0].x,pkPolygon2[1].x),pkPolygon2[2].x);
	kMax2.y = Max(Max(pkPolygon2[0].y,pkPolygon2[1].y),pkPolygon2[2].y);
	kMax2.z = Max(Max(pkPolygon2[0].z,pkPolygon2[1].z),pkPolygon2[2].z);
		
	if(kMin2.x > kMax1.x)
		return false; 
	if(kMin2.y > kMax1.y)
		return false; 
	if(kMin2.z > kMax1.z)
		return false; 

	if(kMin1.x > kMax2.x)
		return false; 
	if(kMin1.y > kMax2.y)
		return false; 
	if(kMin1.z > kMax2.z)
		return false; 

	return true;			
}

bool Tcs::CollideBoxVSBox(P_Tcs* pkBody1,P_Tcs* pkBody2,Tcs_collission* pkTempCol)
{
	bool bCollided = false;

	Matrix4 kBody1Matrix = pkBody1->GetModelMatrix();
	Matrix4 kBody2Matrix = pkBody2->GetModelMatrix();

	Vector3 kBoxEdges1[] = {	//top
									Vector3(-0.5,0.5,-0.5)	,Vector3(0.5,0.5,-0.5),
									Vector3(0.5,0.5,-0.5)	,Vector3(0.5,0.5,0.5),
									Vector3(0.5,0.5,0.5)		,Vector3(-0.5,0.5,0.5),
									Vector3(-0.5,0.5,0.5)	,Vector3(-0.5,0.5,-0.5),
									
									//bottom
									Vector3(0.5,-0.5,-0.5)	,Vector3(-0.5,-0.5,-0.5),
									Vector3(-0.5,-0.5,-0.5)	,Vector3(-0.5,-0.5,0.5),
									Vector3(-0.5,-0.5,0.5)	,Vector3( 0.5,-0.5,0.5),
									Vector3(0.5,-0.5,0.5)	,Vector3(0.5,-0.5,-0.5),
									
									//sides
									Vector3(-0.5,-0.5,0.5)	,Vector3(-0.5,0.5,0.5),
									Vector3(0.5,0.5,0.5)		,Vector3(0.5,-0.5,0.5),
									
									Vector3(0.5,-0.5,-0.5)	,Vector3(0.5,0.5,-0.5),
									Vector3(-0.5,0.5,-0.5)	,Vector3(-0.5,-0.5,-0.5)
																										};		

	Vector3 kBoxEdges2[] = {	//top
									Vector3(-0.5,0.5,-0.5)	,Vector3(0.5,0.5,-0.5),
									Vector3(0.5,0.5,-0.5)	,Vector3(0.5,0.5,0.5),
									Vector3(0.5,0.5,0.5)		,Vector3(-0.5,0.5,0.5),
									Vector3(-0.5,0.5,0.5)	,Vector3(-0.5,0.5,-0.5),
									
									//bottom
									Vector3(0.5,-0.5,-0.5)	,Vector3(-0.5,-0.5,-0.5),
									Vector3(-0.5,-0.5,-0.5)	,Vector3(-0.5,-0.5,0.5),
									Vector3(-0.5,-0.5,0.5)	,Vector3( 0.5,-0.5,0.5),
									Vector3(0.5,-0.5,0.5)	,Vector3(0.5,-0.5,-0.5),
									
									//sides
									Vector3(-0.5,-0.5,0.5)	,Vector3(-0.5,0.5,0.5),
									Vector3(0.5,0.5,0.5)		,Vector3(0.5,-0.5,0.5),
									
									Vector3(0.5,-0.5,-0.5)	,Vector3(0.5,0.5,-0.5),
									Vector3(-0.5,0.5,-0.5)	,Vector3(-0.5,-0.5,-0.5)
																										};																										

	Vector3 kNormals[6] = {
									Vector3(0,1,0),		//up
									Vector3(0,-1,0),		//down
									Vector3(0,0,1),		//front
									Vector3(0,0,-1),		//back
									Vector3(-1,0,0),		//left
									Vector3(1,0,0)			//right
														};			
	
	Vector3 kNormals2[6] = {
									Vector3(0,1,0),		//up
									Vector3(0,-1,0),		//down
									Vector3(0,0,1),		//front
									Vector3(0,0,-1),		//back
									Vector3(-1,0,0),		//left
									Vector3(1,0,0)			//right
														};																
	//transform box edges																										  
	for(int i = 0;i<24;i++)
	{
		kBoxEdges1[i] = kBody1Matrix.VectorTransform((kBoxEdges1[i].PEP(pkBody1->m_kBoxSize)));	
		kBoxEdges2[i] = kBody2Matrix.VectorTransform((kBoxEdges2[i].PEP(pkBody2->m_kBoxSize)));	
	}
	
	//transform box Normals
	for(int i = 0;i<6;i++)
	{
		kNormals[i] = pkBody1->m_kNewRotation.VectorRotate(kNormals[i]);
		kNormals2[i] = pkBody2->m_kNewRotation.VectorRotate(kNormals2[i]);
	}																																								

	
	
	
	// 2 vs 1
	for(int i = 0;i<24;i+=2)
	{
		//top
		if(TestLineVSQuad(&kBoxEdges1[0],&kBoxEdges1[2],&kBoxEdges1[4],&kBoxEdges1[6],&kBoxEdges2[i],&kBoxEdges2[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(-kNormals[0]);
			bCollided = true;
		}
		
		//botom
		if(TestLineVSQuad(&kBoxEdges1[8],&kBoxEdges1[10],&kBoxEdges1[12],&kBoxEdges1[14],&kBoxEdges2[i],&kBoxEdges2[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(-kNormals[1]);
			bCollided = true;
		}
		
		//front
		if(TestLineVSQuad(&kBoxEdges1[6],&kBoxEdges1[4],&kBoxEdges1[14],&kBoxEdges1[12],&kBoxEdges2[i],&kBoxEdges2[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(-kNormals[2]);
			bCollided = true;
		}
		
		//back
		if(TestLineVSQuad(&kBoxEdges1[2],&kBoxEdges1[0],&kBoxEdges1[10],&kBoxEdges1[8],&kBoxEdges2[i],&kBoxEdges2[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(-kNormals[3]);
			bCollided = true;
		}
		
		//left
		if(TestLineVSQuad(&kBoxEdges1[0],&kBoxEdges1[6],&kBoxEdges1[12],&kBoxEdges1[10],&kBoxEdges2[i],&kBoxEdges2[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(-kNormals[4]);
			bCollided = true;
		}
		
		//right
		if(TestLineVSQuad(&kBoxEdges1[4],&kBoxEdges1[2],&kBoxEdges1[8],&kBoxEdges1[14],&kBoxEdges2[i],&kBoxEdges2[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(-kNormals[5]);
			bCollided = true;
		}
	}

	// 1 vs 2
	for(int i = 0;i<24;i+=2)
	{
		//top
		if(TestLineVSQuad(&kBoxEdges2[0],&kBoxEdges2[2],&kBoxEdges2[4],&kBoxEdges2[6],&kBoxEdges1[i],&kBoxEdges1[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(kNormals2[0]);
			bCollided = true;
		}
		
		//botom
		if(TestLineVSQuad(&kBoxEdges2[8],&kBoxEdges2[10],&kBoxEdges2[12],&kBoxEdges2[14],&kBoxEdges1[i],&kBoxEdges1[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(kNormals2[1]);
			bCollided = true;
		}
		
		//front
		if(TestLineVSQuad(&kBoxEdges2[6],&kBoxEdges2[4],&kBoxEdges2[14],&kBoxEdges2[12],&kBoxEdges1[i],&kBoxEdges1[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(kNormals2[2]);
			bCollided = true;
		}
		
		//back
		if(TestLineVSQuad(&kBoxEdges2[2],&kBoxEdges2[0],&kBoxEdges2[10],&kBoxEdges2[8],&kBoxEdges1[i],&kBoxEdges1[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(kNormals2[3]);
			bCollided = true;
		}
		
		//left
		if(TestLineVSQuad(&kBoxEdges2[0],&kBoxEdges2[6],&kBoxEdges2[12],&kBoxEdges2[10],&kBoxEdges1[i],&kBoxEdges1[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(kNormals2[4]);
			bCollided = true;
		}
		
		//right
		if(TestLineVSQuad(&kBoxEdges2[4],&kBoxEdges2[2],&kBoxEdges2[8],&kBoxEdges2[14],&kBoxEdges1[i],&kBoxEdges1[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(kNormals2[5]);
			bCollided = true;
		}
	}
	
																																																						
	return bCollided;
}

bool Tcs::CollideBoxVSMesh(P_Tcs* pkBox,P_Tcs* pkMesh,Tcs_collission* pkTempCol)
{
	static Vector3 verts1[3];	
	static Plane P1;
	static Plane P2;
	static Vector3 Normal1;
	static Vector3 Normal2;
	bool bCollided = false;

	Matrix4 kBoxMatrix = pkBox->GetModelMatrix();
	Matrix4 kMeshMatrix = pkMesh->GetModelMatrix();

	Vector3 kBoxEdges[] = {	//top
									Vector3(-0.5,0.5,-0.5)	,Vector3(0.5,0.5,-0.5),
									Vector3(0.5,0.5,-0.5)	,Vector3(0.5,0.5,0.5),
									Vector3(0.5,0.5,0.5)		,Vector3(-0.5,0.5,0.5),
									Vector3(-0.5,0.5,0.5)	,Vector3(-0.5,0.5,-0.5),
									
									//bottom
									Vector3(0.5,-0.5,-0.5)	,Vector3(-0.5,-0.5,-0.5),
									Vector3(-0.5,-0.5,-0.5)	,Vector3(-0.5,-0.5,0.5),
									Vector3(-0.5,-0.5,0.5)	,Vector3( 0.5,-0.5,0.5),
									Vector3(0.5,-0.5,0.5)	,Vector3(0.5,-0.5,-0.5),
									
									//sides
									Vector3(-0.5,-0.5,0.5)	,Vector3(-0.5,0.5,0.5),
									Vector3(0.5,0.5,0.5)		,Vector3(0.5,-0.5,0.5),
									
									Vector3(0.5,-0.5,-0.5)	,Vector3(0.5,0.5,-0.5),
									Vector3(-0.5,0.5,-0.5)	,Vector3(-0.5,-0.5,-0.5)
																										};
	
	Vector3 kNormals[6] = {
									Vector3(0,1,0),		//up
									Vector3(0,-1,0),		//down
									Vector3(0,0,1),		//front
									Vector3(0,0,-1),		//back
									Vector3(-1,0,0),		//left
									Vector3(1,0,0)			//right
														};																																																				
																									
	//transform box edges																										  
	for(int i = 0;i<24;i++)
	{
		kBoxEdges[i] = kBoxMatrix.VectorTransform((kBoxEdges[i].PEP(pkBox->m_kBoxSize)));	
	}
	
	//transform box Normals
	for(int i = 0;i<6;i++)
	{
		kNormals[i] = pkBox->m_kNewRotation.VectorRotate(kNormals[i]);
		
		//kNormals[i] = Vector3(0,-1,0);
	}
		
	//cout<<"normal:"<<kNormals[1].x<<" "<<kNormals[1].y<<" "<<kNormals[1].z<<endl;
																									  
	//loop trough all mesh vertises																				
	for(unsigned int f=0;f<pkMesh->m_pkFaces->size();f++)
	{
		//transform vertises
		verts1[0] = kMeshMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[0]]);
		verts1[1] = kMeshMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[1]]);		
		verts1[2] = kMeshMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[2]]);		
		
		if(verts1[0] == verts1[1])
			continue;		
		if(verts1[0] == verts1[2])
			continue;
		if(verts1[1] == verts1[2])
			continue;
			
										
		Normal1= ((verts1[1] - verts1[0]).Cross(verts1[2] - verts1[0])).Unit();				
		P1.Set(Normal1,verts1[0]);
		
		//test box VS polygon
		bool bDont = false;
		for(int j = 0;j<24;j+=2)
		{
			if(TestLineVSPolygonNoNormal(verts1,&kBoxEdges[j],&kBoxEdges[j+1],&P1))
			{
				pkTempCol->kPositions.push_back(m_kLastTestPos);
				pkTempCol->kNormals.push_back(Normal1);
				bCollided = true;
				bDont = true;
			}
		}
		
		if(bDont)
			continue;
		
		
		//test polygon VS box (test sides)
		for(int k = 0;k<3;k++)
		{
			int i1,i2;
			switch(k)
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
		
			//top
			if(TestLineVSQuad(&kBoxEdges[0],&kBoxEdges[2],&kBoxEdges[4],&kBoxEdges[6],&verts1[i1],&verts1[i2]))
			{
				pkTempCol->kPositions.push_back(m_kLastTestPos);
				//pkTempCol->kNormals.push_back(-kNormals[0]);
				pkTempCol->kNormals.push_back(-kNormals[0]);
				bCollided = true;
			}
			
			//botom
			if(TestLineVSQuad(&kBoxEdges[8],&kBoxEdges[10],&kBoxEdges[12],&kBoxEdges[14],&verts1[i1],&verts1[i2]))
			{
				pkTempCol->kPositions.push_back(m_kLastTestPos);
				pkTempCol->kNormals.push_back(-kNormals[1]);
				bCollided = true;
			}
			
			//front
			if(TestLineVSQuad(&kBoxEdges[6],&kBoxEdges[4],&kBoxEdges[14],&kBoxEdges[12],&verts1[i1],&verts1[i2]))
			{
				pkTempCol->kPositions.push_back(m_kLastTestPos);
				pkTempCol->kNormals.push_back(-kNormals[2]);
				bCollided = true;
			}
			
			//back
			if(TestLineVSQuad(&kBoxEdges[2],&kBoxEdges[0],&kBoxEdges[10],&kBoxEdges[8],&verts1[i1],&verts1[i2]))
			{
				pkTempCol->kPositions.push_back(m_kLastTestPos);
				pkTempCol->kNormals.push_back(-kNormals[3]);
				bCollided = true;
			}
			
			//left
			if(TestLineVSQuad(&kBoxEdges[0],&kBoxEdges[6],&kBoxEdges[12],&kBoxEdges[10],&verts1[i1],&verts1[i2]))
			{
				pkTempCol->kPositions.push_back(m_kLastTestPos);
				pkTempCol->kNormals.push_back(-kNormals[4]);
				bCollided = true;
			}
			
			//right
			if(TestLineVSQuad(&kBoxEdges[4],&kBoxEdges[2],&kBoxEdges[8],&kBoxEdges[14],&verts1[i1],&verts1[i2]))
			{
				pkTempCol->kPositions.push_back(m_kLastTestPos);
				pkTempCol->kNormals.push_back(-kNormals[5]);
				bCollided = true;
			}
			
		}
	}
	
	
	
	return bCollided;
}

bool Tcs::TestLineVSQuad(Vector3* pkVertex0,Vector3* pkVertex1,Vector3* pkVertex2,Vector3* pkVertex3,Vector3* pkPos1,Vector3* pkPos2)
{
	static Plane kPlane;
	static Vector3 kNormal;
	static Vector3 kV[4];		
	
	kNormal = ((*pkVertex1 - *pkVertex0).Cross(*pkVertex3 - *pkVertex0)).Unit();				
	kPlane.Set(kNormal,*pkVertex0);
	
		
	if(kPlane.LineTest(*pkPos1,*pkPos2,&m_kLastTestPos))
	{
		float fTotAngle = 0;
	
		
		kV[0] = (*pkVertex0 - m_kLastTestPos);
		kV[1] = (*pkVertex1 - m_kLastTestPos);
		kV[2] = (*pkVertex2 - m_kLastTestPos);
		kV[3] = (*pkVertex3 - m_kLastTestPos);		

		
		if(!kV[0].IsZero()) kV[0].Normalize();
		if(!kV[1].IsZero()) kV[1].Normalize();
		if(!kV[2].IsZero()) kV[2].Normalize();
		if(!kV[3].IsZero()) kV[3].Normalize();
				
		fTotAngle += kV[0].Angle(kV[1]);
		fTotAngle += kV[1].Angle(kV[2]);
		fTotAngle += kV[2].Angle(kV[3]);
		fTotAngle += kV[3].Angle(kV[0]);
	
			
		if(fTotAngle >= (1.95*PI))
		{
			//cout<<"inside"<<fTotAngle<<endl;
			return true;
		}
		else
		{
			//cout<<"not inside "<<fTotAngle<<endl;
		}
	}
	
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

			if(!TestPolygonAABB(verts1,verts2))
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

	static float fMinDist = 0.04;

	if(pkPlane->LineTest(*pkPos1,*pkPos2,&m_kLastTestPos))
	{
		if(TestSides(pkPolygon,&(pkPlane->m_kNormal),m_kLastTestPos))
		{
			//m_kLastTestNormal = -pkPlane->m_kNormal;		
			//m_kLastTestNormal = kNormal1;
			//return true;
			
			if( m_kLastTestPos.DistanceTo(*pkPos1) < fMinDist ||
				 m_kLastTestPos.DistanceTo(*pkPos2) < fMinDist) //|| 
//				 m_kLastTestPos.DistanceTo(pkPolygon[0]) < fMinDist ||
//				 m_kLastTestPos.DistanceTo(pkPolygon[1]) < fMinDist ||
//				 m_kLastTestPos.DistanceTo(pkPolygon[2]) < fMinDist)
			
			{			
				//vertex VS face
				m_kLastTestNormal = -pkPlane->m_kNormal;
				
				return true;
			}
			else if( m_kLastTestPos.DistanceTo(pkPolygon[0]) < fMinDist ||
				 		m_kLastTestPos.DistanceTo(pkPolygon[1]) < fMinDist ||
				 		m_kLastTestPos.DistanceTo(pkPolygon[2]) < fMinDist)
			{
				//cout<<"hora"<<endl;
				m_kLastTestNormal = kNormal1;
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

				float d = 0;
															
				if((f1 < f2) && (f1 < f3))
				{
					d = f1;
					m_kLastTestNormal = n1;
				};
				if((f2 < f1) && (f2 < f3))
				{
					d = f2;
					m_kLastTestNormal = n2;
				};
				if((f3 < f1) && (f3 < f2))
				{
					d = f3;
					m_kLastTestNormal = n3;
				};			
				/*
				//check if theres an edge vs edge collision
				if(d > 0.01)
				{
					m_kLastTestNormal = -pkPlane->m_kNormal;
					return true;
				}	
				*/
				
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
		{
			m_kLastTestPos = kTestPos;
			return true;
		}
		
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
	if((pkBody1->m_kLinearVelocity.Length() < m_fSleepLinVel) &&
		(pkBody1->m_kRotVelocity.Length() <  m_fSleepRotVel) &&
		(pkBody2->m_kLinearVelocity.Length() < m_fSleepLinVel) &&
		(pkBody2->m_kRotVelocity.Length() <  m_fSleepRotVel) )		
	{
	
		pkBody1->Sleep();
		pkBody2->Sleep();
		
		if(pkBody1->m_bSleeping)
		{
			if(pkBody1->m_bDisableOnSleep)
			{
				pkBody1->Disable();
				pkBody1->GetEntity()->DeleteProperty("P_Tcs");;
			}
		}
			
		if(pkBody2->m_bSleeping)
		{
			if(pkBody2->m_bDisableOnSleep)
			{
				pkBody2->Disable();
				pkBody2->GetEntity()->DeleteProperty("P_Tcs");
			}
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
