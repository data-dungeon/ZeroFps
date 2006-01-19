#include "tcs.h"
#include "../propertys/p_tcs.h"
#include "../common/heightmap.h"
	
Tcs::Tcs(): ZFSubSystem("Tcs")
{
	g_Logf("zerofps","Tiny Collission system created");

	m_fSleepLinVel = 	0.5;
	m_fSleepRotVel = 	0.3;
	m_fMaxVel = 		15.0;
	m_fGravity =		9.84;
	
	
	m_iNrOfCollissions = 0;
	m_iNrOfTests = 		0;
	m_iNrOfContacts =		0;
	
	m_pkBodyCopy1 	=	NULL;
	m_pkBodyCopy2	=	NULL;
	
	m_kiHandleCollission.Register(this, "p_tcshandle", "1");
	m_kiDebugGraph.Register(this, "p_tcsdebug", "0");
	m_kfSleepTime.Register(this, "p_tcssleeptime", "1.0");
	m_kiCollisionIterations.Register(this, "p_tcscolit", "4");
	m_kiContactIterations.Register(this, "p_tcsconit", "4");
}

Tcs::~Tcs()
{

}


bool Tcs::StartUp()	
{ 
	m_pkZeroFps		= static_cast<ZSSZeroFps*>(GetSystem().GetObjectPtr("ZSSZeroFps"));		
	m_pkRender		= static_cast<ZSSRender*>(GetSystem().GetObjectPtr("ZSSRender"));		
	m_pkEntityMan	= static_cast<ZSSEntityManager*>(GetSystem().GetObjectPtr("ZSSEntityManager"));		
	
	m_pkBodyCopy1 = new P_Tcs();
	m_pkBodyCopy2 = new P_Tcs();	
		
	return true; 
}

bool Tcs::ShutDown() 
{
	//	 blir ju massa evil krasher h� s�..wtf f� la va lite minnes lekage s�l�ge =D
	delete m_pkBodyCopy1;	
	delete m_pkBodyCopy2;
	

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

void Tcs::AddTrigger(P_TcsTrigger* pkPTcsTrigger)
{
// 	cout<<"adding trigger"<<endl;

 	//check so its not added twice
	for(unsigned int i=0;i<m_kTriggers.size();i++)
	{
		if(m_kTriggers[i] == pkPTcsTrigger)
			return;
	
	}
	
	m_kTriggers.push_back(pkPTcsTrigger);
}
void Tcs::RemoveTrigger(P_TcsTrigger* pkPTcsTrigger)
{
	for(unsigned int i=0;i<m_kTriggers.size();i++)
	{
		if(m_kTriggers[i] == pkPTcsTrigger)
		{	
			m_kTriggers[i]=m_kTriggers.back();			//change place whit bak elment 
			m_kTriggers.pop_back();
			
			return;
		}
	}
}

void Tcs::Update(float fAlphaTime)
{
	if(m_kBodys.empty())
		return;
	
// 	StartProfileTimer("s__tcs");				
	
	static float fLastTime= 0; 		
	static float fStepSize = 0.033333;
	
	m_iNrOfCollissions	= 0;
	m_iNrOfTests			= 0;
	m_iNrOfActiveBodies	= 0;	
	m_iNrOfContacts		= 0;

	
	fStepSize = m_pkZeroFps->GetEngineTime() - fLastTime;
	fLastTime = m_pkZeroFps->GetEngineTime();
	
	//fStepSize = fAlphaTime;
	
	if(fStepSize <= 0)
		return;
	
	//minimum time size
	if(fStepSize > 0.05)
		fStepSize = 0.05;
	
	
	//synd all bodys to entitys
	SyncBodys();
	
	
// 	StartProfileTimer("s__tcs-main");
// MAIN LOOP
	
	//calculate current forces
	UpdateForces();		
	
		
	//handle collisions
	for(int i = 0;i< m_kiCollisionIterations.GetInt();i++)
	{
		//save old positions and velocitys		
		PushVelPos();
		
		//update velocity and position
		UpdateBodyVels(fStepSize);					
		UpdateBodyPos(fStepSize);
		
		//do collissiontests
		ClearCollissions();				
		UpdateCollissions(&m_kCollissions);
	
		PopVelPos();			

 		//breake if there was no collissions
		if(m_kCollissions.size() == 0)
 			break;		
		
		//handle collission
		m_iNrOfCollissions+=m_kCollissions.size();
		HandleCollissions();
					
	}
	
	m_iNrOfContacts = m_kCollissions.size();
	//cout<<"total collissions:"<<iTotalCollissions<<" contactcs remaining:"<<m_kCollissions.size()<<endl;
		
	//apply forces on velocity
	UpdateBodyVels(fStepSize);		

	//update all character ground line tests	
	UpdateLineTests(fStepSize);				
	
	//handle contacts
	for(int i = 0;i< m_kiContactIterations.GetInt();i++)
	{
		//save old positions and velocitys
		PushVelPos();
		
		//update body pos
		UpdateBodyPos(fStepSize);
		
		//do collissiontests
		ClearCollissions();				
		UpdateCollissions(&m_kCollissions);
		
		PopVelPos();			

 		//breake if there was no collissions
		if(m_kCollissions.size() == 0)
 			break;				
		
		//handle collission
		StopObjects(fStepSize);
							
	}
	
	//do shock test
	Shock();				
		
	//now move all objects	
	UpdateBodyPos(fStepSize);
//---------------- End of maaaaaaaaaain loop	
	
// 	StopProfileTimer("s__tcs-main");
	

	//check triggers
	UpdateTriggers();
	
	//check for resting bodys
 	static float fLastRestFind = 0;	
 	if(m_pkZeroFps->GetEngineTime() - fLastRestFind > 0.1)
 	{
 		fLastRestFind = m_pkZeroFps->GetEngineTime();
 		FindRestingBodys();
 	}
		
	//synd all entitys to bodys
	SyncEntitys();
	
	//clear all forces
	ResetForces();
	
	
// 	StopProfileTimer("s__tcs");			
}


void Tcs::UpdateTriggers()
{
	static P_Tcs* pkBody;
	static P_TcsTrigger* pkTrigger;
	static int iBodys;
	static int iTriggers;
	static Vector3 kPos;
	
	iTriggers = m_kTriggers.size();
	iBodys = m_kBodys.size();

	if(iTriggers == 0)
		return;
	
	//loop all bodys
	for(unsigned int i=0;i<iBodys;i++)
	{	
		pkBody  = m_kBodys[i];
					
		if(pkBody->m_bStatic)
			continue;

		//sett all triggers to negative value
		for(map<int,bool>::iterator it = pkBody->m_kTrigging.begin();it != pkBody->m_kTrigging.end();it++)
			(*it).second = false;
		

		//for each body loop all triggers
		for(unsigned int j=0;j<iTriggers;j++)
		{	
			pkTrigger = m_kTriggers[j];			
			
			kPos = pkTrigger->GetEntity()->GetWorldPosV();		
			
						
			switch(pkTrigger->m_iTriggerType)
			{
				case eSPHERE:
					if(pkBody->m_kNewPos.DistanceTo(kPos) < pkTrigger->m_fRadius)
					{
						//not trigged?
						if(pkBody->m_kTrigging.find(pkTrigger->GetEntity()->GetEntityID()) == pkBody->m_kTrigging.end())
							pkTrigger->Trigger(pkBody);
						
						pkBody->m_kTrigging[pkTrigger->GetEntity()->GetEntityID()] = true;							
					}
					break;

				case eBOX:
					if(pkBody->m_kNewPos.x > kPos.x-pkTrigger->m_kBoxSize.x && pkBody->m_kNewPos.x < kPos.x+pkTrigger->m_kBoxSize.x &&
						pkBody->m_kNewPos.y > kPos.y-pkTrigger->m_kBoxSize.y && pkBody->m_kNewPos.y < kPos.y+pkTrigger->m_kBoxSize.y &&
						pkBody->m_kNewPos.z > kPos.z-pkTrigger->m_kBoxSize.z && pkBody->m_kNewPos.z < kPos.z+pkTrigger->m_kBoxSize.z)
					{
						//not trigged?
						if(pkBody->m_kTrigging.find(pkTrigger->GetEntity()->GetEntityID()) == pkBody->m_kTrigging.end())
							pkTrigger->Trigger(pkBody);
						
						pkBody->m_kTrigging[pkTrigger->GetEntity()->GetEntityID()] = true;
					}	
					break;								
			}					
		}
		
		//check if any triggers has ben released
		for(map<int,bool>::iterator it = pkBody->m_kTrigging.begin();it != pkBody->m_kTrigging.end();it++)
		{
			if((*it).second == false)
			{
				map<int,bool>::iterator bakup = it;				
				it++;								
				pkBody->m_kTrigging.erase(bakup);				
				
				if(it == pkBody->m_kTrigging.end())
					break;
			}		
		}
	}
}

void Tcs::PushVelPos()
{
	static P_Tcs* pkBody;
	static int iBodys;

	iBodys = m_kBodys.size();
 	for(unsigned int i=0;i<iBodys;i++)
 	{
		pkBody = m_kBodys[i];	
	
		if(pkBody->InActive())
			continue;
			
		pkBody->m_kOldNewPos = pkBody->m_kNewPos;
		pkBody->m_kOldLinearVelocity = pkBody->m_kLinearVelocity;
		
		pkBody->m_kOldNewRotation = pkBody->m_kNewRotation;
		pkBody->m_kOldRotVelocity = pkBody->m_kRotVelocity;	
	}
}

void Tcs::PopVelPos()
{
	static P_Tcs* pkBody;
	static int iBodys;

	iBodys = m_kBodys.size();
 	for(unsigned int i=0;i<iBodys;i++)
 	{
		pkBody = m_kBodys[i];	
		
		if(pkBody->InActive())
			continue;

				
		pkBody->m_kNewPos = pkBody->m_kOldNewPos;
		pkBody->m_kLinearVelocity = pkBody->m_kOldLinearVelocity;
		
		pkBody->m_kNewRotation = pkBody->m_kOldNewRotation;
		pkBody->m_kRotVelocity = pkBody->m_kOldRotVelocity;	
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
		int iSize = m_kCollissions.size();
		for(int i = 0;i<iSize;i++)
		{
			//check if both are now static
			if(	(m_kCollissions[i]->pkBody1->InActive()) && 
					(m_kCollissions[i]->pkBody2->InActive())	)
					continue;
		
			//is one static?
			if(	m_kCollissions[i]->pkBody1->InActive()
				||	m_kCollissions[i]->pkBody2->InActive())
			{
				//handle collission as a bouncing collision but no angular effect
// 				HandleCollission(m_kCollissions[i],false,true);					
					HandleCollission(m_kCollissions[i],true,true);					

				
				if(m_kCollissions[i]->pkBody1->InActive())
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
		if(m_kBodys[i]->InActive())
			continue;
			
			
					
		if(m_kBodys[i]->m_bCharacter)
		{			
			m_kBodys[i]->m_bOnGround = false;					
			 
			if(CharacterLineTest(m_kBodys[i]->m_kNewPos,Vector3(0,-1,0),m_kBodys[i]))
			{		
				distance = fabs(m_kBodys[i]->m_kNewPos.y - m_kLastLineTestColPos.y);
				
				if(distance <= m_kBodys[i]->m_fLegLength)
				{
// 					float fNewYVel = ((m_kBodys[i]->m_fLegLength - distance) / fAlphaTime);
					
					float fNewYVel = (m_kBodys[i]->m_fLegLength - distance) * 250 * fAlphaTime;
// 					m_kBodys[i]->m_kLinearVelocity.y += fAlphaTime;
					
// 					if(fNewYVel > 1.0)
// 						fNewYVel = 1.0;	
					
// 					cout<<fNewYVel<<endl;
					
//  					cout<<"oldvel:"<<m_kBodys[i]->m_kLinearVelocity.y<<"   newvel:"<<fNewYVel<<endl;
//  					cout<<"bla"<<fAlphaTime<<" "<<(m_kBodys[i]->m_fLegLength - distance)<<endl;
 					
 					if(m_kBodys[i]->m_kLinearVelocity.y < fNewYVel)
 					{
 						m_kBodys[i]->m_kLinearVelocity.y = fNewYVel;
 					
						m_kBodys[i]->m_bOnGround = true; 					
 					}
 					
 					
//  					m_kBodys[i]->m_kLinearVelocity.y = Min((m_kBodys[i]->m_fLegLength - distance) * 60.0,50.0);
//  					m_kBodys[i]->m_kLinearVelocity.y = fabs(m_kBodys[i]->m_kLinearVelocity.y)*2.0;
//  					cout<<"vel:"<<m_kBodys[i]->m_kLinearVelocity.y<<endl;
				}
			}			
		}
	}
}

void Tcs::HandleCollission(Tcs_collission* pkCol,bool bNoBounce,bool bNoAngular)
{	
	//if no collission respons is wanted, just do touch and return 
	if(pkCol->pkBody1->m_bNoColRespons || pkCol->pkBody2->m_bNoColRespons)
	{
		SyncEntitys();
		pkCol->pkBody1->GetEntity()->Touch(pkCol->pkBody2->GetEntity()->GetEntityID());
		pkCol->pkBody2->GetEntity()->Touch(pkCol->pkBody1->GetEntity()->GetEntityID());	
		SyncBodys();	
		return;
	}
	
	
	
	//wake up bodys
	pkCol->pkBody1->Wakeup();
	pkCol->pkBody2->Wakeup();

	// masses
	float fMass1 = pkCol->pkBody1->m_fMass;
	float fMass2 = pkCol->pkBody2->m_fMass;
	
	//treat static bodys as having infinit mass	
	if(pkCol->pkBody1->m_bStatic || pkCol->pkBody1->m_bTempStatic)
	{
		fMass1 = 999999999;	
	}
	
	if(pkCol->pkBody2->m_bStatic || pkCol->pkBody2->m_bTempStatic)
	{
		fMass2 = 999999999;	
	}
		
		
	//bounce	
// 	float fBounce = pkCol->pkBody1->m_fBounce * pkCol->pkBody2->m_fBounce;
	float fBounce = Max(pkCol->pkBody1->m_fBounce,pkCol->pkBody2->m_fBounce);
	
	if(bNoBounce)
		fBounce = 0.1;
	
	//friction , we use the maximum friction coefficient
	//float fFriction = pkCol->pkBody1->m_fFriction * pkCol->pkBody2->m_fFriction;
	float fFriction = Max(pkCol->pkBody1->m_fFriction,pkCol->pkBody2->m_fFriction);
	
	//setup vels	
	Vector3 kLinearRelVel = pkCol->pkBody1->m_kLinearVelocity - pkCol->pkBody2->m_kLinearVelocity;
	Vector3 kRotVel1 = pkCol->pkBody1->m_kRotVelocity;
	Vector3 kRotVel2 = pkCol->pkBody2->m_kRotVelocity;

	//nr of collision points
	int iNrOfPos = pkCol->kPositions.size();
	
			
	//handle all collision points
	for(int i = 0;i<	iNrOfPos;i++)
	{	
		if(m_kiDebugGraph.GetInt())
			m_pkRender->Line(pkCol->kPositions[i],pkCol->kPositions[i] + pkCol->kNormals[i]);		
		
		//collission handling is disabled
		if(!m_kiHandleCollission.GetInt())
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
// 		int iLoops = 0;
// 		while(	pkCol->kNormals[i].Dot(
// 				( (pkCol->pkBody1->m_kLinearVelocity-pkCol->pkBody2->m_kLinearVelocity) + 
// 				(-pkCol->pkBody1->m_kRotVelocity.Cross(pkCol->kPositions[i] - pkCol->pkBody1->m_kNewPos) - 
// 				-pkCol->pkBody2->m_kRotVelocity.Cross(pkCol->kPositions[i] - pkCol->pkBody2->m_kNewPos)) )) < 0	)
// 		{					
// 			iLoops++;			
// 			if(iLoops >= 5)			
// 				break;
						
			// APPLY IMPULSES					
			if( (!pkCol->pkBody1->m_bStatic) && (!pkCol->pkBody1->m_bTempStatic) && (pkCol->pkBody1->m_bActive) )
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
			
			if( (!pkCol->pkBody2->m_bStatic) && (!pkCol->pkBody2->m_bTempStatic) && (pkCol->pkBody2->m_bActive) )
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
// 		}
	}

	
	//pkCol->pkBody1->GetEntity()->Touch(pkCol->pkBody2->GetEntity()->GetEntityID());
	//pkCol->pkBody2->GetEntity()->Touch(pkCol->pkBody1->GetEntity()->GetEntityID());	
	
	//sync entitys before touching	
	SyncEntitys();
	//touch objects	
	pkCol->pkBody1->GetEntity()->Touch(pkCol->pkBody2->GetEntity()->GetEntityID());
	pkCol->pkBody2->GetEntity()->Touch(pkCol->pkBody1->GetEntity()->GetEntityID());	
	//sync bodys after touching
	SyncBodys();
	
	//try to put bodys to sleep
	//TryToSleep(pkCol->pkBody1,pkCol->pkBody2);						
}

void Tcs::FindRestingBodys()
{	
	static float fRestMaxDist = 0.2;

	//update move distance
	for(int i = 0;i<m_kBodys.size();i++)
	{
		if(m_kBodys[i]->InActive())
			continue;
	
		//update bodys total movement
		m_kBodys[i]->m_fMoveDistance += m_kBodys[i]->m_kNewPos.DistanceTo(m_kBodys[i]->m_kLastPos);
		m_kBodys[i]->m_kLastPos = m_kBodys[i]->m_kNewPos;
			
	}
	
	//check for resting bodys
	static float fLastRestFind = 0;	
	if(m_pkZeroFps->GetEngineTime() - fLastRestFind > m_kfSleepTime.GetFloat())
	{
		fLastRestFind = m_pkZeroFps->GetEngineTime();
	
		for(int i = 0;i<m_kBodys.size();i++)
		{
			if(m_kBodys[i]->InActive())
				continue;
			
			//check if body has moved less then fRestMaxDist since last check
			if(m_kBodys[i]->m_fMoveDistance <= fRestMaxDist)
			{			
			
				m_kBodys[i]->Sleep();
			
				if(m_kBodys[i]->m_bDisableOnSleep)
				{
					m_kBodys[i]->Disable();
					m_kBodys[i]->GetEntity()->DeleteProperty("P_Tcs");;
				}
				
			}	
			
			//clear move distance
			m_kBodys[i]->m_fMoveDistance = 0;			
			
		}
	}
	
				
		
			
/*		if(m_kBodys[i]->m_kNewPos.DistanceTo(m_kBodys[i]->m_kLastPos) <= fRestMaxDist)
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
		m_kBodys[i]->m_kLastPos = m_kBodys[i]->m_kNewPos;*/
		
// 	}
}


void Tcs::SyncEntitys()
{
	static P_Tcs* pkBody;
	static int iBodys;
	
	iBodys = m_kBodys.size();
	for(unsigned int i=0;i<iBodys;i++)
	{			
		pkBody = m_kBodys[i];
			
		pkBody->GetEntity()->SetWorldPosV(pkBody->m_kNewPos);
		//pkBody->GetEntity()->SetVel(m_kBodys[i]->m_kLinearVelocity);
		pkBody->GetEntity()->SetLocalRotM(pkBody->m_kNewRotation);
	}
}

void Tcs::SyncBodys()
{
	static P_Tcs* pkBody;
	static int iBodys;
	
	iBodys = m_kBodys.size();
	for(unsigned int i=0;i<iBodys;i++)
	{
		pkBody = m_kBodys[i];	
	
		pkBody->m_kNewPos = pkBody->GetEntity()->GetWorldPosV();		
		pkBody->m_kNewRotation = pkBody->GetEntity()->GetLocalRotM();
		//m_kBodys[i]->m_kLinearVelocity = m_kBodys[i]->GetObject()->GetVel();							
	}
}

void Tcs::UpdateBodyVels(float fAtime)
{
	static int iBodys;
	static P_Tcs** pkFirstBody;
	
	pkFirstBody = &m_kBodys[0];
	iBodys = m_kBodys.size();

	for(unsigned int i=0;i<iBodys;i++)
	{	
		UpdateBodyVel(pkFirstBody[i],fAtime);					
	}
}

void Tcs::UpdateBodyVel(P_Tcs* pkBody,float fAtime)
{
	if(pkBody->InActive())
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
	static P_Tcs* pkBody;
	static int iBodys;
	
	iBodys = m_kBodys.size();
	for(unsigned int i=0;i<iBodys;i++)
	{	
		pkBody = m_kBodys[i];
	
		if(pkBody->InActive())
			continue;

			
		//Calculate new position
		pkBody->m_kNewPos += (pkBody->m_kLinearVelocity * fAtime);
		//calculate new rotation 
		pkBody->m_kNewRotation.RadRotate(pkBody->m_kRotVelocity * fAtime);					
	}
}


void Tcs::UpdateForces()
{
	static P_Tcs* pkBody;
	static int iBodys;

	iBodys = m_kBodys.size();
	for(unsigned int i=0;i<iBodys;i++)
	{	
		pkBody = m_kBodys[i];
	
		if(pkBody->InActive())
			continue;

		//increse active body count
		m_iNrOfActiveBodies++;
			
				
		// LINEAR FORCE / ACCLERERATION			
			pkBody->m_kLinearForce.Set(0,0,0);
		
			//add external linear force
			pkBody->m_kLinearForce += pkBody->m_kExternalLinearForce; 
					
			//apply gravity if enabled
			if(pkBody->m_bGravity)
				pkBody->m_kLinearForce.y -= m_fGravity * pkBody->m_fMass;


							
			//apply some air friction		
			pkBody->m_kLinearForce -= pkBody->m_kLinearVelocity * pkBody->m_fAirFriction;
	
			//Calculate linear acceleration
			pkBody->m_kLinearForce = pkBody->m_kLinearForce / pkBody->m_fMass;	
				
		
		
		// ROTATIONAL FORCE / ACCLERATION			
			pkBody->m_kRotForce.Set(0,0,0);
			
			//add external rotation force
			pkBody->m_kRotForce += pkBody->m_kExternalRotForce; 		
			
			//apply some air friction				
			pkBody->m_kRotForce -= pkBody->m_kRotVelocity * (pkBody->m_fAirFriction*2);
	
			//calculate rotation acceleration
			pkBody->m_kRotForce = pkBody->m_kRotForce * pkBody->m_fInertia;
	
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
	if(pkBody->InActive())
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
	static P_Tcs* pkBody1;
	static P_Tcs* pkBody2;
	static int iBodys;
	
	//first update all bounding boxes
	UpdateAABBs();

	//loop trough all bodys and testing for collisions against each other
	P_Tcs** pkBodyStart = &m_kBodys[0];
	iBodys = m_kBodys.size();
	for(unsigned int B1=0;B1<iBodys;B1++)
	{
		pkBody1 = pkBodyStart[B1];
	
		for(unsigned int B2=B1+1;B2<iBodys;B2++)
		{					
			pkBody2 = pkBodyStart[B2];
		
			//dont test if both bodys are static or sleeping
			if( (pkBody1->InActive()) && (pkBody2->InActive()))
				continue;		
				
			//check collission groups
			if( !(pkBody1->m_akTestGroups[pkBody2->m_iGroup] ||
				pkBody2->m_akTestGroups[pkBody1->m_iGroup]) )			
				continue;
				
			//check if entitys are in neighbour zone
			if(!IsInNerbyZone(pkBody1,pkBody2))
				continue;				
								
			//do sphere test early
			if( (pkBody1->m_iTestType==E_SPHERE) && (pkBody2->m_iTestType==E_SPHERE) ) 
			{
				//SPHERE VS SPHERE
				TestSphereVsSphere(pkBody1,pkBody2,pkCollissions);												
				m_iNrOfTests++;
				continue;
			}
								
								
			//first do a sphere test			
			if(!CollideSphereVSSphere(pkBody1,pkBody2))
				continue;
			
			//do aabb test	
			if(!TestAABB(pkBody1,pkBody2))
				continue;
			
			//increse number of tests
			m_iNrOfTests++;

			
		
			if( ((pkBody1->m_iTestType==E_MESH) && (pkBody2->m_iTestType==E_SPHERE)) ||
					((pkBody2->m_iTestType==E_MESH) && (pkBody1->m_iTestType==E_SPHERE)))
			{				
				//SPHERE VS MESH
				TestSphereVsMesh(pkBody1,pkBody2,pkCollissions);
			}
			else if	(((pkBody1->m_iTestType==E_SPHERE) && (pkBody2->m_iTestType==E_HMAP)) || 
						((pkBody1->m_iTestType==E_HMAP) && (pkBody2->m_iTestType==E_SPHERE)))
			{
				//SPHERE VS MESH ( HEIGHTMAP)
				TestSphereVsMesh(pkBody1,pkBody2,pkCollissions);			
			}
			else if(((pkBody1->m_iTestType==E_BOX) && (pkBody2->m_iTestType==E_MESH)) || 
				((pkBody1->m_iTestType==E_MESH) && (pkBody2->m_iTestType==E_BOX)))
			{
				//BOX VS MESH
				TestBoxVsMesh(pkBody1,pkBody2,pkCollissions);
			}
			else if(((pkBody1->m_iTestType==E_BOX) && (pkBody2->m_iTestType==E_SPHERE)) || 
				((pkBody1->m_iTestType==E_SPHERE) && (pkBody2->m_iTestType==E_BOX)))
			{
				//BOX VS SPHERE
				TestBoxVsSphere(pkBody1,pkBody2,pkCollissions);
			}			
			else if( (pkBody1->m_iTestType==E_BOX) && (pkBody2->m_iTestType==E_BOX) )
			{
				//BOX VS BOX
				TestBoxVsBox(pkBody1,pkBody2,pkCollissions);
			}
			else if( (pkBody1->m_iTestType==E_MESH) && (pkBody2->m_iTestType==E_MESH) )
			{
				//MESH VS MESH
				TestMeshVsMesh(pkBody1,pkBody2,pkCollissions);
			}				
 			else if	(((pkBody1->m_iTestType==E_MESH) && (pkBody2->m_iTestType==E_HMAP)) || 
 						((pkBody1->m_iTestType==E_HMAP) && (pkBody2->m_iTestType==E_MESH)))
 			{
 				//HMAP VS MESH
 				TestMeshVsMesh(pkBody1,pkBody2,pkCollissions);			
 			}
		}
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
	static int iBodys;
	static int iVertexes;
		
	iBodys = m_kBodys.size();
	for(int i = 0;i<iBodys;i++)
	{
		P_Tcs* pkBody = m_kBodys[i];
	
		//if body is static or sleeping check if rotation has changes, if not dont update
		if(pkBody->InActive())
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
					
					//want to test nexttime to
					pkBody->m_kAABBRotation.Zero();
					break;
				}				
				
				//try to setup AABB tree on static objects
				if(pkBody->m_bStatic)
				{
					pkBody->m_kAABBTree.Create(pkBody->m_pkFaces,
														pkBody->m_pkVertex,
														pkBody->m_pkNormal,
														pkBody->m_kNewRotation,
														pkBody->m_fScale
														);
				}
				else
					pkBody->m_kAABBTree.Clear();
				
				
				
				pkBody->m_kAABBMax.Set(-9999999,-9999999,-9999999);	
				pkBody->m_kAABBMin.Set(9999999,9999999,9999999);	
				
				iVertexes = pkBody->m_pkVertex->size();
				for(unsigned int f=0;f<iVertexes;f++)
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
	static int iZoneID1,iZoneID2;
	static int iZoneLinks;
	static ZoneData* pkZoneData;
	
	iZoneID1 = pkBody1->GetEntity()->GetCurrentZone();
	iZoneID2 = pkBody2->GetEntity()->GetCurrentZone();
		
	
	if(iZoneID1 == iZoneID2)
		return true;
	
	if(pkZoneData = m_pkEntityMan->GetZoneData(iZoneID1))
	{
		iZoneLinks = pkZoneData->m_iZoneLinks.size();
		for(int i = 0;i<iZoneLinks;i++)
		{
			if(pkZoneData->m_iZoneLinks[i] == iZoneID2)
				return true;
		}
	}
	
	return false;
}

bool Tcs::TestSides(const Vector3* kVerts,const Vector3* pkNormal,const Vector3& kPos)
{
  
	//   first way to do it :D
	static Vector3 vert0p;
	static Vector3 vert1p;
	static Vector3 vert2p;

	vert0p = kVerts[0] - kPos;
	vert1p = kVerts[1] - kPos;
	float d = vert0p.Cross(vert1p).Dot(*pkNormal);
	if (d < 0) return false;
	vert2p = kVerts[2] - kPos;
	d = vert1p.Cross(vert2p).Dot(*pkNormal);
	if (d < 0) return false;
	d = vert2p.Cross(vert0p).Dot(*pkNormal);
	if (d < 0) return false;
	return true;



// 	//one way to do it
// 	static Vector3 e10,e20,vp;
// 	static float a,b,c,ac_bb,d,e;	
// 	
// 	e10=kVerts[1]-kVerts[0];
// 	e20=kVerts[2]-kVerts[0];
// 	a = e10.Dot(e10);
// 	b = e10.Dot(e20);
// 	c = e20.Dot(e20);
// 	ac_bb=(a*c)-(b*b);
// 	vp.Set(kPos.x-kVerts[0].x, kPos.y-kVerts[0].y, kPos.z-kVerts[0].z);
// 	d = vp.Dot(e10);
// 	e = vp.Dot(e20);
// 	
// 	float x = (d*c)-(e*b);
// 	float y = (e*a)-(d*b);
// 	float z = x+y-ac_bb;
//  	return (( ((unsigned int &)z) & ~(((unsigned int &)x)|((unsigned int &)y)) ) & 0x80000000) != 0;
// 	

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
	static float fClosest;
	static float d;
	static int iBodys; 
	static Vector3 kPos2;
	static P_Tcs* pkClosest;	
	static P_Tcs* pkBody;
	
	m_kLastLineTestColPos =	kStart;		
	fClosest = 					999999999;
	pkClosest =					NULL;	
	kPos2 = 						kStart + kDir*10;
	
	iBodys = m_kBodys.size();
	for(unsigned int i=0;i<iBodys;i++)
	{
		pkBody = m_kBodys[i];
					
		if(	(pkBody == pkTester) || 
				(!pkTester->m_akWalkableGroups[pkBody->m_iGroup]) )
			continue;

		//check if entitys are in neighbour zone
		if(!IsInNerbyZone(pkTester,pkBody))
			continue;							
		
		switch(pkBody->m_iTestType)
		{
			case E_HMAP:
			case E_MESH:
				if(CharacterTestLineVSSphere(kStart,kPos2,pkBody))
				{
					if(CharacterTestLineVSMesh(kStart,kDir,pkBody))
					{
						d = kStart.DistanceTo(m_kLastTestPos);				
						if(d < fClosest)
						{						
							m_kLastLineTestColPos = m_kLastTestPos;
							fClosest = d;
							pkClosest = pkBody;
						}													
					}					
				}
				break;
/*				
			case E_HMAP:
				if(CharacterTestLineVSSphere(kStart,kPos2,pkBody))
					if(CharacterTestLineVSMesh(kStart,kDir,pkBody))
					{
						d = kStart.DistanceTo(m_kLastTestPos);				
						if(d < fClosest)
						{						
							m_kLastLineTestColPos = m_kLastTestPos;
							fClosest = d;
							pkClosest = pkBody;
						}													
					}					
				break;*/
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

bool Tcs::CharacterTestLineVSMesh(const Vector3& kStart,const Vector3& kDir,P_Tcs* pkMesh)
{
	static Vector3 kPoint2;
	
	kPoint2 	= kStart + kDir * 1000;
	

	if(pkMesh->m_kAABBTree.IsValid())
	{
		if(pkMesh->m_kAABBTree.m_pkRootNode->TestLine(kStart - pkMesh->m_kNewPos,kPoint2 - pkMesh->m_kNewPos,&m_kLastTestPos))
		{
			m_kLastTestPos += pkMesh->m_kNewPos;
			return true;
		}
		else
			return false;
	}
	else
	{
		
		static Matrix4 kModelMatrix;
		static float closest;
		static bool bHaveColided;
		static Vector3 kClosestNormal;
		static Vector3 kClosestPos;
		static Vector3 verts[3];	
		static int iFaces;
		static float d;
		
		kModelMatrix 	= pkMesh->GetModelMatrix();	
		closest 			= 99999999;
		bHaveColided 	= false;		
	
			
		iFaces = pkMesh->m_pkFaces->size();
		for(unsigned int f=0;f<iFaces;f++)
		{		 
			verts[0] = kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[0]]);
			verts[1] = kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[1]]);		
			verts[2] = kModelMatrix.VectorTransform((*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[2]]);		
			
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
}

bool Tcs::TestLineVSPolygon(const Vector3* pkVerts,const Vector3* pkPos1,const Vector3* pkPos2)
{
	static Plane P;
	static Vector3 V1,V2,Normal;

	V1 = pkVerts[1] - pkVerts[0];
	V2 = pkVerts[2] - pkVerts[0];		
	Normal= V1.Cross(V2);
	
	if(Normal.IsZero())
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

bool Tcs::CharacterTestLineVSSphere(const Vector3& kP1,const Vector3& kP2,P_Tcs* pkB)
{
	static Vector3 kDir;
	static Vector3 c;
	static Vector3 k;	
	static float d,cdis,kdis,Distance;
	
	kDir = kP2 - kP1;
	c=pkB->m_kNewPos - kP1;		

	kDir.Normalize();		
	
	d = kDir.Dot(c);
	k=kDir.Proj(c);		
	cdis=c.Length();
	kdis=k.Length();
	Distance = sqrt( (cdis*cdis) - (kdis*kdis) );
	
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

bool Tcs::TestLineVSSphere(const Vector3& kP1,const Vector3& kP2,P_Tcs* pkB)
{
	static Vector3 kDir,c,k;
	static float d,cdis,kdis,Distance;
	

	kDir = kP2 - kP1;
	c=pkB->m_kNewPos - kP1;		

	d = kDir.Unit().Dot(c);
	if(d < 0)
		return false;
	if(d > kDir.Length())
		return false;
	
	kDir.Normalize();		
	k=kDir.Proj(c);		
	cdis=c.Length();
	kdis=k.Length();
	Distance = (float) sqrt((cdis*cdis)-(kdis*kdis));
	

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
}

bool Tcs::CollideSphereVSSphere(P_Tcs* pkBody1,P_Tcs* pkBody2)
{
	return (	pkBody1->m_kNewPos.DistanceTo(pkBody2->m_kNewPos) - 
				(pkBody1->m_fRadius + pkBody2->m_fRadius) ) <= 0;
}


void Tcs::TestSphereVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,vector<Tcs_collission*>*	pkCollissions)
{
	//if body1 is the mesh just flip them,body2 shuld always be the mesh
	if( (pkBody1->m_iTestType==E_MESH) || pkBody1->m_iTestType==E_HMAP )
	{
		P_Tcs* b2c = pkBody2;	
		pkBody2 = pkBody1;
		pkBody1 = b2c;
	}
	
	//make sure body2 have all the pointers else get angry and cry
	if(!pkBody2->m_bHavePolygonData)
		return;
	
	if(pkBody2->m_kAABBTree.IsValid())
	{
		if(CollideSphereVSAABBTree(pkBody1,pkBody2))
		{
			Tcs_collission* temp = new Tcs_collission;
			
			temp->pkBody1 = pkBody1;
			temp->pkBody2 = pkBody2;	
			temp->kNormals.push_back((pkBody1->m_kNewPos - m_kLastTestPos).Unit());
			temp->kPositions.push_back(m_kLastTestPos);
						
			pkCollissions->push_back(temp);		
		}
	}
	else
	{	
		if(CollideSphereVSMesh(pkBody1,pkBody2))
		{
			Tcs_collission* temp = new Tcs_collission;
			
			temp->pkBody1 = pkBody1;
			temp->pkBody2 = pkBody2;	
			temp->kNormals.push_back((pkBody1->m_kNewPos - m_kLastTestPos).Unit());
			temp->kPositions.push_back(m_kLastTestPos);
						
			pkCollissions->push_back(temp);				
		}
	}
}

bool Tcs::CollideSphereVSAABBTree(P_Tcs* pkSphere,P_Tcs* pkMesh)
{

	if(pkMesh->m_kAABBTree.m_pkRootNode->TestSphere(pkSphere->m_kNewPos - pkMesh->m_kNewPos,pkSphere->m_fRadius,&m_kLastTestPos))
	{
  		m_kLastTestPos += pkMesh->m_kNewPos;
		return true;
	}
	
	return false;
}

bool Tcs::CollideSphereVSMesh(P_Tcs* pkSphere,P_Tcs* pkMesh)
{
	static Matrix4 kModelMatrix;
	static Vector3 kClosestNormal;
	static Vector3 kClosestPos;
	static Vector3 verts[3];		 
	static float closest,d;
	static bool bHaveColided;
	static int iFaces;	 
	
	kModelMatrix = pkMesh->GetModelMatrix();	
	closest = 99999999;
	bHaveColided = false;	
	d;

	iFaces = pkMesh->m_pkFaces->size();
	for(unsigned int f=0;f<iFaces;f++)
	{		 
		verts[0] = kModelMatrix.VectorTransform( (*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[0]]);
		verts[1] = kModelMatrix.VectorTransform( (*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[1]]);		
		verts[2] = kModelMatrix.VectorTransform( (*pkMesh->m_pkVertex)[(*pkMesh->m_pkFaces)[f].iIndex[2]]);		
		
		if(m_kiDebugGraph.GetInt() == 2)
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
	static Vector3 kPos1,V1,V2,Normal,kColPos;
	static Plane P;
	

	kPos1 = pkSphere->m_kNewPos;

	V1 = kVerts[1] - kVerts[0];
	V2 = kVerts[2] - kVerts[0];		
	Normal= V1.Cross(V2);
	
	if(Normal.IsZero())
	{
		return false;
	}
	
	Normal.Normalize();
	P.m_fD = -Normal.Dot(kVerts[0]);	
	P.m_kNormal = Normal;
	
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
	static bool didcollide;
	static float closest;
	static int p1,p2;
	static float d;
	
	didcollide = false;
	closest = 99999999;	
	
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

void Tcs::TestBoxVsSphere(P_Tcs* pkBox,P_Tcs* pkSphere,vector<Tcs_collission*>*	pkCollissions)
{
	if(pkSphere->m_iTestType == E_BOX)
	{
		P_Tcs* b2c = pkSphere;	
		pkSphere = pkBox;
		pkBox = b2c;	
	}	


	static Matrix4 kIRM;
	static Vector3 kSpherePos;
	
	kIRM = pkBox->m_kNewRotation.Inverse();
	kSpherePos = kIRM.VectorTransform(pkSphere->m_kNewPos - pkBox->m_kNewPos);
	
	kSpherePos += pkBox->m_kNewPos;

	static Plane kPlanes[6];

	kPlanes[0].Set(Vector3(1,0,0),	Vector3(pkBox->m_kNewPos.x+pkBox->m_kBoxSize.x,	pkBox->m_kNewPos.y,	pkBox->m_kNewPos.z));
	kPlanes[1].Set(Vector3(-1,0,0),	Vector3(pkBox->m_kNewPos.x-pkBox->m_kBoxSize.x,	pkBox->m_kNewPos.y,	pkBox->m_kNewPos.z));
	kPlanes[2].Set(Vector3(0,1,0),	Vector3(pkBox->m_kNewPos.x,	pkBox->m_kNewPos.y+pkBox->m_kBoxSize.y,pkBox->m_kNewPos.z));
	kPlanes[3].Set(Vector3(0,-1,0),	Vector3(pkBox->m_kNewPos.x,	pkBox->m_kNewPos.y-pkBox->m_kBoxSize.y,pkBox->m_kNewPos.z));
	kPlanes[4].Set(Vector3(0,0,1),	Vector3(pkBox->m_kNewPos.x,	pkBox->m_kNewPos.y,	pkBox->m_kNewPos.z+pkBox->m_kBoxSize.z));
	kPlanes[5].Set(Vector3(0,0,-1),	Vector3(pkBox->m_kNewPos.x,	pkBox->m_kNewPos.y,	pkBox->m_kNewPos.z-pkBox->m_kBoxSize.z));

	float fD = 0;
	int t = 0;
	for(int i =0;i<6;i++)
	{
		float d = kPlanes[i].SphereTest(kSpherePos,pkSphere->m_fRadius);
	
		if(d > 0)
		{
			return;		
		}
		
		if(d < fD)
		{
			fD = d;
			t = i;
		}
	}
	
	//cout<<"box vs sphere collission "<<t<<endl;
	
	Tcs_collission* temp = new Tcs_collission;
	temp->Clear();
	
	temp->pkBody1 = pkBox;
	temp->pkBody2 = pkSphere;		
		
// 	temp->kPositions.push_back(pkSphere->m_kNewPos - pkBox->m_kNewRotation.VectorTransform(kPlanes[t].m_kNormal)*pkSphere->m_fRadius);
//   	temp->kNormals.push_back(pkBox->m_kNewRotation.VectorTransform(kPlanes[t].m_kNormal));	
//    	temp->kNormals.push_back( (pkSphere->m_kNewPos - temp->kPositions[0]).Unit());	
  	temp->kNormals.push_back( (pkBox->m_kNewPos - pkSphere->m_kNewPos).Unit());	
 	temp->kPositions.push_back(pkSphere->m_kNewPos - temp->kNormals[0]*pkSphere->m_fRadius);  	
//  	temp->kNormals.push_back(pkBox->m_kNewRotation.VectorTransform(kPlanes[t].m_kNormal));
		
	pkCollissions->push_back(temp);			
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
		
		if(m_kiDebugGraph.GetInt() == 2)
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
							
			if(m_kiDebugGraph.GetInt() == 2)
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
	static Matrix4 kBody1Matrix;
	static Matrix4 kBody2Matrix;
 	
	static Vector3 kBoxEdges1[24];
 	static Vector3 kBoxEdges2[24];
	static Vector3 kNormals1[6];
	static Vector3 kNormals2[6];
	
	
	bool bCollided = false;

	kBody1Matrix = pkBody1->GetModelMatrix();
	kBody2Matrix = pkBody2->GetModelMatrix();

// 	//top
// 	kBoxEdges1[0].Set(-0.5,0.5,-0.5);	kBoxEdges1[1].Set(0.5,0.5,-0.5);
// 	kBoxEdges1[2].Set(0.5,0.5,-0.5);		kBoxEdges1[3].Set(0.5,0.5,0.5);
// 	kBoxEdges1[4].Set(0.5,0.5,0.5);		kBoxEdges1[5].Set(0.5,0.5,0.5);
// 	kBoxEdges1[6].Set(-0.5,0.5,0.5);		kBoxEdges1[7].Set(-0.5,0.5,-0.5);
// 
// 	//bottom
// 	kBoxEdges1[8].Set(0.5,-0.5,-0.5);	kBoxEdges1[9].Set(-0.5,-0.5,-0.5);
// 	kBoxEdges1[10].Set(-0.5,-0.5,-0.5);	kBoxEdges1[11].Set(-0.5,-0.5,0.5);
// 	kBoxEdges1[12].Set(-0.5,-0.5,0.5);	kBoxEdges1[13].Set( 0.5,-0.5,0.5);
// 	kBoxEdges1[14].Set(0.5,-0.5,0.5);	kBoxEdges1[15].Set(0.5,-0.5,-0.5);
// 	
// 	//sides
// 	kBoxEdges1[14].Set(-0.5,-0.5,0.5);	kBoxEdges1[15].Set(-0.5,0.5,0.5);
// 	kBoxEdges1[14].Set(0.5,0.5,0.5);		kBoxEdges1[15].Set(0.5,-0.5,0.5);
// 	
// 	kBoxEdges1[14].Set(0.5,-0.5,-0.5);	kBoxEdges1[15].Set(0.5,0.5,-0.5);
// 	kBoxEdges1[14].Set(-0.5,0.5,-0.5);	kBoxEdges1[15].Set(-0.5,-0.5,-0.5);
		
	
	
	static Vector3 kOrgBoxEdges1[] = {//top
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

	static Vector3 kOrgBoxEdges2[] = {	//top
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

	static Vector3 kOrgNormals1[] = {
												Vector3(0,1,0),		//up
												Vector3(0,-1,0),		//down
												Vector3(0,0,1),		//front
												Vector3(0,0,-1),		//back
												Vector3(-1,0,0),		//left
												Vector3(1,0,0)			//right
														};			
	
	static Vector3 kOrgNormals2[] = {
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
		kBoxEdges1[i] = kBody1Matrix.VectorTransform((kOrgBoxEdges1[i].PEP(pkBody1->m_kBoxSize)));	
		kBoxEdges2[i] = kBody2Matrix.VectorTransform((kOrgBoxEdges2[i].PEP(pkBody2->m_kBoxSize)));	
	}
	
	//transform box Normals
	for(int i = 0;i<6;i++)
	{
		kNormals1[i] = pkBody1->m_kNewRotation.VectorRotate(kOrgNormals1[i]);
		kNormals2[i] = pkBody2->m_kNewRotation.VectorRotate(kOrgNormals2[i]);
	}																																								

	
	
	
	// 2 vs 1
	for(int i = 0;i<24;i+=2)
	{
		//top
		if(TestLineVSQuad(&kBoxEdges1[0],&kBoxEdges1[2],&kBoxEdges1[4],&kBoxEdges1[6],&kBoxEdges2[i],&kBoxEdges2[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(-kNormals1[0]);
			bCollided = true;
		}
		
		//botom
		if(TestLineVSQuad(&kBoxEdges1[8],&kBoxEdges1[10],&kBoxEdges1[12],&kBoxEdges1[14],&kBoxEdges2[i],&kBoxEdges2[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(-kNormals1[1]);
			bCollided = true;
		}
		
		//front
		if(TestLineVSQuad(&kBoxEdges1[6],&kBoxEdges1[4],&kBoxEdges1[14],&kBoxEdges1[12],&kBoxEdges2[i],&kBoxEdges2[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(-kNormals1[2]);
			bCollided = true;
		}
		
		//back
		if(TestLineVSQuad(&kBoxEdges1[2],&kBoxEdges1[0],&kBoxEdges1[10],&kBoxEdges1[8],&kBoxEdges2[i],&kBoxEdges2[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(-kNormals1[3]);
			bCollided = true;
		}
		
		//left
		if(TestLineVSQuad(&kBoxEdges1[0],&kBoxEdges1[6],&kBoxEdges1[12],&kBoxEdges1[10],&kBoxEdges2[i],&kBoxEdges2[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(-kNormals1[4]);
			bCollided = true;
		}
		
		//right
		if(TestLineVSQuad(&kBoxEdges1[4],&kBoxEdges1[2],&kBoxEdges1[8],&kBoxEdges1[14],&kBoxEdges2[i],&kBoxEdges2[i+1]))
		{
			pkTempCol->kPositions.push_back(m_kLastTestPos);
			pkTempCol->kNormals.push_back(-kNormals1[5]);
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
	static Vector3 kBoxEdges[24];
	static Vector3 kNormals[6];
	
	bool bCollided = false;
	Matrix4 kBoxMatrix = pkBox->GetModelMatrix();
	Matrix4 kMeshMatrix = pkMesh->GetModelMatrix();

	static Vector3 kOrgBoxEdges[] = {	//top
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
	
	static Vector3 kOrgNormals[] = {
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
		kBoxEdges[i] = kBoxMatrix.VectorTransform((kOrgBoxEdges[i].PEP(pkBox->m_kBoxSize)));	
	}
	
	//transform box Normals
	for(int i = 0;i<6;i++)
	{
		kNormals[i] = pkBox->m_kNewRotation.VectorRotate(kOrgNormals[i]);
		
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
	
			
		if(fTotAngle >= (1.95*Math::PI))
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
	static Matrix4 kModelMatrix1;
	static Matrix4 kModelMatrix2;

	
	static Vector3 verts1[3];	
	static Vector3 verts2[3];	
	static Plane P1;
	static Plane P2;
	static Vector3 Normal1;
	static Vector3 Normal2;
	static int i1,i2;

	static bool bT1,bT2,bT3;

	//bool bHaveCleared = false;		
	kModelMatrix1 = pkBody1->GetModelMatrix();
	kModelMatrix2 = pkBody2->GetModelMatrix();	
	bool bHaveColided = false;	
	
		
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
			
		if(m_kiDebugGraph.GetInt() == 2)
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
			
			//test if all vertises are on one side of the other polygon plane	
			bT1 = P1.PointInside(verts2[0]);
			bT2 = P1.PointInside(verts2[1]);
			bT3 = P1.PointInside(verts2[2]);
			if( (bT1&&bT2&&bT3) || (!bT1&&!bT2&&!bT3) )
				continue;

								
			if(m_kiDebugGraph.GetInt() == 2)
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
	static Matrix4 kModelMatrix2;
	static Vector3 verts[3];
	static Vector3 kNormal;	
	
	kModelMatrix2 = pkBody->GetModelMatrix();
	

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
