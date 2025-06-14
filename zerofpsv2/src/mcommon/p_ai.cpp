#include "p_ai.h"
#include "../zerofpsv2/engine_systems/propertys/p_pfpath.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../zerofpsv2/engine/entity.h"
#include "../zerofpsv2/engine/zerofps.h"
#include <iostream>
#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h"
#include "p_charactercontrol.h"
#include "p_characterproperty.h"
#include "../zerofpsv2/basic/math.h"

using namespace std;

#define EVENT_Update			1
#define EVENT_Enter			2
#define EVENT_Exit			3

#define BeginStateMachine	if(iState < 0){if(0){
#define EndStateMachine		return(true);}}else{ZFAssert(0, "Unknown AI State"); \
									return(false);}return(false);
#define State(a)				return(true);}}else if(a == iState){if(0){
#define OnEvent(a)			return(true);}else if(a == iEvent){
#define OnUpdate				OnEvent(EVENT_Update)
#define OnEnter				OnEvent(EVENT_Enter)
#define OnExit					OnEvent(EVENT_Exit)


P_AI::P_AI(): Property("P_AI")
{


	m_pkRender=				static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));			

	m_iSide		= PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;
	m_iType		= PROPERTY_TYPE_NORMAL|PROPERTY_TYPE_RENDER;
	m_bNetwork 	= false;
	m_iVersion	= 2;
	
	
	m_pkCharacterControl = NULL;
	m_pkCharacterProperty = NULL;
	
	m_iState				= eAI_STATE_GUARD;
	m_iNextState		= eAI_STATE_NONE;
	m_iPrevState		= eAI_STATE_NONE;
	m_bStateChanged	= false;

	m_kGuardPos = Vector3(0,0,0);
	m_bHaveGuardPos = false;

	m_fSeeDistance = 		16;
	m_fAttackDistance =	10;
	m_fStrikeRange = 		2;
	
	m_bWalk 		= true;
	m_fTime 		= 0;
	m_iTarget	= -1;
	
	m_fFindTime = 0;
	m_fCollisionTime = 0;
	
	SetState(eAI_STATE_GUARD);


	m_kPropertyValues.push_back(PropertyValues("state",VALUETYPE_INT,(void*)&m_iState));

}

P_AI::~P_AI()
{


}


void P_AI::Init()
{
	m_fTime = m_pkZeroFps->GetEngineTime() + Math::Randomf(5);

}

void P_AI::DrawCircle(float fRadius, char* szEditColor)
{
	Vector3	kVertex;
	vector<Vector3>	kVertexList;
	Vector3 kCenterPos = GetEntity()->GetIWorldPosV();

	for(int i=0; i<360; i+=(int)12.25) 
	{
		kVertex.x = float( cos(Math::DegToRad( float(i) )) * fRadius );
		kVertex.z = float( sin(Math::DegToRad( float(i) )) * fRadius );
		kVertex.y = 0;
		
		kVertex += kCenterPos;
		kVertexList.push_back(kVertex);
	}

	m_pkRender->DrawCircle(kVertexList, m_pkRender->GetEditColor(szEditColor) );

}

void P_AI::DrawEditor()
{
	if(!m_pkZeroFps->m_kAI_ShowInfo.GetBool())
		return;

	DrawCircle(m_fStrikeRange,		"ai/rngstrike");
	DrawCircle(m_fAttackDistance, "ai/rngattack");
	DrawCircle(m_fSeeDistance,		"ai/rngsee");
}

void P_AI::Update() 
{


	if(!m_pkEntity->InActiveZone())
		return;

	if(m_pkZeroFps->m_bEditMode && m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER))
	{
		DrawEditor();
		return;
	}

	//get character control
	if(!m_pkCharacterControl)
	{
		if(!(m_pkCharacterControl = (P_CharacterControl*)m_pkEntity->GetProperty("P_CharacterControl")))
		{
			cout<<"WARNING: P_AI missing P_CharacterControl"<<endl;
			return;		
		}
		
		m_pkCharacterControl->SetYAngle(Math::Randomf(360));								
	}

	//get characterproperty
	if(!m_pkCharacterProperty)
	{
		if(!(m_pkCharacterProperty = (P_CharacterProperty*)m_pkEntity->GetProperty("P_CharacterProperty")))
		{
			cout<<"WARNING: P_AI missing P_CharacterProperty"<<endl;
			return;		
		}
		
	}
				
		
	//AI distance
	if(m_pkEntityManager->DistanceToTracker(m_pkEntity->GetWorldPosV()) > m_pkZeroFps->m_kAI_Distance.GetFloat())
	{
		m_pkCharacterControl->SetControl(eUP,false);
		return;	
	}
	
	
	m_fStrikeRange = GetOffensiveRange();	
	Process(EVENT_Update);
	
}


void P_AI::Process(int iEvent)
{
	
	if(m_iState != eAI_STATE_AVOIDING && m_pkZeroFps->GetEngineTime() > m_fCollisionTime + 3)
	{
		m_fCollisionTime = m_pkZeroFps->GetEngineTime();
		Vector3 kNormal;
		if(CharacterCollision(&kNormal))
			SetState(eAI_STATE_AVOIDING);
	}

	States(iEvent, m_iState);	

	// Check if state has changed and send events
	int iSaftyCount = 10;
	while(m_bStateChanged && (--iSaftyCount >= 0))
	{
		ZFAssert( iSaftyCount > 0, "AI States are unstable");
		
		m_bStateChanged = false;
		States(EVENT_Exit, m_iState);
		m_iPrevState = m_iState;
		m_iState = m_iNextState;
		States(EVENT_Enter, m_iState);
	}
}

bool P_AI::States(int iEvent, int iState)
{
	BeginStateMachine
		State(eAI_STATE_AVOIDING)
			OnEnter
				m_pkCharacterProperty->DebugSet("AiState", "eAI_STATE_AVOIDING");
				Vector3 kNormal;
				if(CharacterCollision(&kNormal))
				{
					m_pkCharacterControl->RotateTowards(m_pkEntity->GetWorldPosV() - kNormal);
				}											
									
			OnUpdate
				if(m_pkZeroFps->GetEngineTime() > m_fCollisionTime + 0.25)
				{
					m_fCollisionTime = m_pkZeroFps->GetEngineTime();
					
					Vector3 kNormal;
					if(CharacterCollision(&kNormal))
						m_pkCharacterControl->RotateTowards(m_pkEntity->GetWorldPosV() - kNormal);
					else
						SetState(m_iPrevState);
									
				}
				
				m_pkCharacterControl->SetControl(eCRAWL,true);						
				m_pkCharacterControl->SetControl(eUP,true);
	
			OnExit
				m_pkCharacterControl->SetControl(eCRAWL,false);						
				m_pkCharacterControl->SetControl(eUP,false);
	
		State(eAI_STATE_RANDOMWALK)
			OnEnter
				m_pkCharacterProperty->DebugSet("AiState", "eAI_STATE_RANDOMWALK");
			OnUpdate
				if(m_bWalk)
				{
				
					float fRot = m_pkCharacterControl->GetYAngle();				
					fRot += Math::Randomf(20)-10;
					m_pkCharacterProperty->SetCombatMode(false);
					m_pkCharacterControl->SetYAngle(fRot);								
					m_pkCharacterControl->SetControl(eUP,true);
					m_pkCharacterControl->SetControl(eCRAWL,true);											
				
					if(m_pkZeroFps->GetEngineTime() > m_fTime + 2)
					{
						m_bWalk = false;
						m_fTime = m_pkZeroFps->GetEngineTime();
					}
				}
				else
				{
					m_pkCharacterControl->SetControl(eUP,false);					
				
					if(m_pkZeroFps->GetEngineTime() > m_fTime + 5)
					{
						m_bWalk = true;
						m_fTime = m_pkZeroFps->GetEngineTime();					
					}			
				}

				//look for enemy , and attack
				if(m_pkZeroFps->GetEngineTime() > m_fFindTime + 1)
				{
					m_fFindTime = m_pkZeroFps->GetEngineTime();
				
					int iEnemy = FindClosestEnemy(m_fAttackDistance);
					if(iEnemy != -1)
					{
						//set attack state
						SetState( eAI_STATE_CHASE );
						m_iTarget = iEnemy;
						
					}
				}

		State(eAI_STATE_LOOKAT)
			OnEnter
				m_pkCharacterProperty->DebugSet("AiState", "eAI_STATE_LOOKAT");

			OnUpdate
				if(!ValidTarget(m_iTarget))
				{
					SetState( eAI_STATE_GUARD );
					return false;
				}		
			
				if(Entity* pkEnemy = m_pkEntityManager->GetEntityByID(m_iTarget))
				{
					float fDistance = pkEnemy->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
				
					if(fDistance > m_fSeeDistance)
					{
						SetState(eAI_STATE_GUARD);
						return false;
					}
				
					//look at
					m_pkCharacterControl->RotateTowards(pkEnemy->GetWorldPosV());
					m_pkCharacterControl->SetControl(eUP,false);						
					//m_pkCharacterControl->SetControl(eCRAWL,true);						
					
					if(fDistance < m_fAttackDistance)
					{
						SetState( eAI_STATE_CHASE );
						return false;
					}
				}
				
				//check for other closer targets
				if(m_pkZeroFps->GetEngineTime() > m_fFindTime + 1)
				{
					m_fFindTime = m_pkZeroFps->GetEngineTime();
				
					m_iTarget = FindClosestEnemy(m_fSeeDistance);
				}
												
				

		State(eAI_STATE_CHASE)
			OnEnter
				m_pkCharacterProperty->DebugSet("AiState", "eAI_STATE_CHASE");

			OnUpdate
				if(!ValidTarget(m_iTarget))
				{
					SetState( eAI_STATE_GUARD );
					return false;
				}		
				
				if(m_fStrikeRange == -1)
				{
					SetState( eAI_STATE_GUARD );
					return false;				
				}
			
			
				if(Entity* pkEnemy = m_pkEntityManager->GetEntityByID(m_iTarget))
				{
					float fDistance = pkEnemy->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
				
					if(fDistance > m_fSeeDistance)
					{
						SetState( eAI_STATE_GUARD ); //return to random walk
						return false;
					}
				
					
					if(fDistance < m_fStrikeRange )
					{
						SetState (eAI_STATE_ATTACK );
						return false;
					}
					
					
					//check for other closer targets
					if(m_pkZeroFps->GetEngineTime() > m_fFindTime + 1)
					{
						m_fFindTime = m_pkZeroFps->GetEngineTime();
					
						int iEnemy = FindClosestEnemy(m_fSeeDistance);
						if(iEnemy != -1)
						{
							//set attack state
							//SetState( eAI_STATE_CHASE );
							//m_iTarget = iEnemy;
							
							if(Entity* pkNew = m_pkEntityManager->GetEntityByID(iEnemy))
							{
								if(pkNew->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV()) < fDistance)
								{
									m_iTarget = iEnemy;
									pkEnemy = pkNew;
								}
							}
						}
					}					
					
					//chase
					m_pkCharacterProperty->SetCombatMode(true);
					
					m_pkCharacterControl->RotateTowards(pkEnemy->GetWorldPosV());
					m_pkCharacterControl->SetControl(eUP,true);
					m_pkCharacterControl->SetControl(eCRAWL,false);						
				}

		State(eAI_STATE_ATTACK)
			OnEnter
				m_pkCharacterProperty->DebugSet("AiState", "eAI_STATE_ATTACK");
				
			OnUpdate
				if(!ValidTarget(m_iTarget))
				{
					SetState(eAI_STATE_GUARD);
					return false;
				}

				if(m_fStrikeRange == -1)
				{
					SetState( eAI_STATE_GUARD );
					return false;				
				}
				
				if(Entity* pkEnemy = m_pkEntityManager->GetEntityByID(m_iTarget))
				{
					float fDistance = pkEnemy->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
				
					if(fDistance > m_fStrikeRange)
					{
						SetState(eAI_STATE_CHASE);
						return false;
					}
							
					m_pkCharacterProperty->SetCombatMode(true);
					//m_pkCharacterProperty->SetTarget(m_iTarget);
					
					//get best skill for current distance
					string strSkill = GetBestSkill(fDistance);
					if(!strSkill.empty())
					{
						//m_pkCharacterProperty->SetDefaultAttackSkill( strSkill);					
						m_pkCharacterProperty->UseSkill(strSkill,m_iTarget,true);					
					
						m_pkCharacterControl->RotateTowards(pkEnemy->GetWorldPosV());
						m_pkCharacterControl->SetControl(eUP,false);									
					}
				}

		State(eAI_STATE_GUARD)
			OnEnter
				m_pkCharacterProperty->DebugSet("AiState", "eAI_STATE_GUARD");				

			OnUpdate
				//look for enemy
				if(m_pkZeroFps->GetEngineTime() > m_fFindTime + 1)
				{
					m_fFindTime = m_pkZeroFps->GetEngineTime();
				
					int iEnemy = FindClosestEnemy(m_fSeeDistance);
					if(iEnemy != -1)
					{
						//set look att state
						if(!m_bHaveGuardPos)
						{
							m_kGuardPos = m_pkEntity->GetWorldPosV();
							m_bHaveGuardPos = true;
						}
						
						SetState(eAI_STATE_LOOKAT);
						m_iTarget = iEnemy;
						return false;
					}	
				}
					
				m_pkCharacterProperty->SetCombatMode(false);					
				m_pkCharacterControl->SetControl(eUP,false);									
					
				if(m_bHaveGuardPos)
				{
					
					if(m_pkEntity->GetWorldPosV().DistanceTo(Vector3(m_kGuardPos.x,m_pkEntity->GetWorldPosV().y,m_kGuardPos.z)) < 0.5)
					{
						m_bHaveGuardPos = false;						
					}
					else
					{
						m_pkCharacterControl->RotateTowards(m_kGuardPos);
						m_pkCharacterControl->SetControl(eUP,true);
						m_pkCharacterControl->SetControl(eCRAWL,true);												
					}
				}
					

		State(eAI_STATE_DEAD)
			OnEnter
				m_pkCharacterProperty->DebugSet("AiState", "eAI_STATE_DEAD");

			OnUpdate

		State(eAI_STATE_SIT)
			OnEnter
				m_pkCharacterProperty->DebugSet("AiState", "eAI_STATE_SIT");
				m_pkCharacterControl->Sit();	
				
			OnUpdate

	EndStateMachine
}


bool P_AI::ValidTarget(int iTarget)
{
	if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iTarget,"P_CharacterProperty"))	
		if(!pkCP->IsDead() && !pkCP->IsIncap() && m_pkCharacterProperty->IsEnemy(iTarget))
			return true;	

	return false;	
}

void P_AI::Touch(int iID)
{
	if(m_pkCharacterControl)
	{	
	
		if(P_CharacterControl* pkCC = (P_CharacterControl*)m_pkEntityManager->GetPropertyFromEntityID(iID,"P_CharacterControl"))
		{
			if(pkCC->GetControl(eUP))
			{
				m_pkCharacterControl->Lock(0.5);
	 			m_pkCharacterControl->SetControl(eUP,false);
			}
		}
		else
		{		
			float fRot = m_pkCharacterControl->GetYAngle();				
			fRot +=180;
			m_pkCharacterControl->SetYAngle(fRot);								
		}
	}
}

string P_AI::GetBestSkill(float fRange)
{
	Skill* pkBestSkill = NULL;
	float fBestRangeDiff = 999999;
	
	vector<Skill*>* pkSkills = m_pkCharacterProperty->GetSkillList();

	int iSkills = pkSkills->size();
	for(int i = 0;i< iSkills;i++)
	{
		if((*pkSkills)[i]->GetSkillType() == eOFFENSIVE)			//is an offesive skill
			if((*pkSkills)[i]->CanUse())									//can use it now
				if(fRange <= (*pkSkills)[i]->GetRange())				//its range is close enought
				{
					float diff = (*pkSkills)[i]->GetRange() - fRange;
					
					//its a better range difference then the privious one
					if(diff < fBestRangeDiff)
					{
						fBestRangeDiff = diff;
						pkBestSkill = (*pkSkills)[i];				
					}
				}
	}	
	
	if(pkBestSkill)
		return pkBestSkill->GetName();
	else
		return string("");
}

float P_AI::GetOffensiveRange()
{
	float fMaxRange = -1;

	vector<Skill*>* pkSkills = m_pkCharacterProperty->GetSkillList();

	int iSkills = pkSkills->size();
	for(int i = 0;i< iSkills;i++)
	{
		if((*pkSkills)[i]->GetSkillType() == eOFFENSIVE )
			if((*pkSkills)[i]->CanUse())					
				if((*pkSkills)[i]->GetRange() > fMaxRange)
					fMaxRange = (*pkSkills)[i]->GetRange();
	}	

	return fMaxRange;
}


bool P_AI::CharacterCollision(Vector3* pkNormal)
{
	vector<Property*> kPropertys;
	Entity* pkZone = NULL;

	//does this entity use zones?
	if(m_pkEntity->GetUseZones())
	{
		pkZone = m_pkEntity->GetParent();
	}
	else if(m_pkEntity->GetParent()->GetUseZones())
	{
		pkZone = m_pkEntity->GetParent()->GetParent();
	}
	
	pkZone->GetAllPropertys(&kPropertys,PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_SERVER);
	
	Vector3 kPos = m_pkEntity->GetWorldPosV();
	int iSize = kPropertys.size();
	for(int i = 0;i<iSize;i++)
	{
		if(kPropertys[i]->GetEntity() == m_pkEntity)
			continue;
		
		if(!kPropertys[i]->IsType("P_CharacterProperty"))
			continue;
		
		if(((P_CharacterProperty*)kPropertys[i])->IsDead() || ((P_CharacterProperty*)kPropertys[i])->IsIncap())
			continue;
		
		if(kPos.DistanceTo(kPropertys[i]->GetEntity()->GetWorldPosV()) < 1.0)
		{		
			*pkNormal = (kPropertys[i]->GetEntity()->GetWorldPosV() - kPos).Unit();
			return true;
		}
	}
	
	return false;
}

int P_AI::FindClosestEnemy(float fMaxRange)
{
	vector<Entity*>	kZones;	
	vector<Property*> kPropertys;
		
	int iEnemy = -1;
	float fRange = 999999999;
	
	Entity* pkZone = NULL;
	
	//does this entity use zones?
	if(m_pkEntity->GetUseZones())
	{
		pkZone = m_pkEntity->GetParent();
	}
	else if(m_pkEntity->GetParent()->GetUseZones())
	{
		pkZone = m_pkEntity->GetParent()->GetParent();
	}
	
	
	//we assume that parent is a zone
	if(pkZone)
	{
		pkZone->GetZoneNeighbours(&kZones);
		
		//get all propertys in zones
		for(int i =0;i<kZones.size();i++)
		{
			kZones[i]->GetAllPropertys(&kPropertys,PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_SERVER);
		}
		
		
		//find characters
		static Property* pkProp;
		
		for(int i = 0;i<kPropertys.size();i++)
		{
			pkProp = kPropertys[i];
		
			//skip self
			if(pkProp == m_pkCharacterProperty)
				continue;
		
			//we only care about character propertys
 			if(!pkProp->IsType("P_CharacterProperty"))
 				continue;
				
			if(P_CharacterProperty* pkCP = static_cast<P_CharacterProperty*>(pkProp))
			{
				if(pkCP->IsDead() || pkCP->IsIncap())
					continue;
				//found character
				
				//is it eeevil?
				if(m_pkCharacterProperty->IsEnemy(pkCP->GetEntity()->GetEntityID()))
				{					
					float fDistance = pkCP->GetEntity()->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
					
					//out of range
					if(fDistance > fMaxRange)
						continue;
					
					//is this one closer than the last one?
					if(fDistance < fRange)
					{
						fRange = fDistance;
						iEnemy = pkCP->GetEntity()->GetEntityID();					
					}
				}
			}
		}
	}
	
	return iEnemy;
}

void P_AI::ChangeState(int iNewState)
{
	m_iNextState		= iNewState;
	m_bStateChanged	= true;
}

void P_AI::SetState(int iNewState)
{
	m_iNextState		= iNewState;
	m_bStateChanged	= true;
}


bool P_AI::HandleSetValue( const string& kValueName, const string& kValue )
{
	if(kValueName == "state")
	{
		SetState(atoi(kValue.c_str()));
		return true;
	}

	return false;
}


void P_AI::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write(m_iState);
	
}

void P_AI::Load(ZFIoInterface* pkPackage,int iVersion)
{
	if(iVersion == 2)
		pkPackage->Read(m_iState);

	SetState(m_iState);
}
// 
// 
// void P_AI::PackTo(NetPacket* pkNetPacket, int iConnectionID )
// {
// }
// 
// 
// void P_AI::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
// {
// }




// SCRIPT INTERFACE FOR P_AI
using namespace ObjectManagerLua;

namespace SI_P_AI
{
/** \class AI 
 *  \ingroup Property
 *  */

/**	\fn SetAITarget( EntityID, TargetEntityID)
		\brief Sets the taget that the ai try to attack.
		\relates AI
*/
	int SetAITargetLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,2))
			return 0;		

			
		int iCharcterID;
		int iTargetID;
		
		g_pkScript->GetArgInt(pkLua, 0,&iCharcterID);
		g_pkScript->GetArgInt(pkLua, 1,&iTargetID);
		
		if(P_AI* pkAI = (P_AI*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_AI"))
		{
			pkAI->SetTarget(iTargetID);
		}
	
		return 0;			
	}

/**	\fn SetAIState( Entity, AI_STATES)
		\brief Sets the current state of the AI
		\relates AI
*/
	int SetAIStateLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,2))
			return 0;		
	
		double dEntID, dAIState;
		g_pkScript->GetArgNumber(pkLua, 0, &dEntID);		
		g_pkScript->GetArgNumber(pkLua, 1, &dAIState);				

		Entity* pkObj = g_pkObjMan->GetEntityByID(int(dEntID));

		if ( !pkObj )
			return 0;

		// Get P_AI
		if(P_AI* pkAI = (P_AI*)pkObj->GetProperty("P_AI"))
			pkAI->SetState(int(dAIState));

		return 0;
	}	
}



Property* Create_P_AI()
{
	return new P_AI;
}

void Register_P_AI(ZSSZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_AI", Create_P_AI);					

	// Register Property Script Interface
 	g_pkScript->ExposeFunction("SetAITarget",	SI_P_AI::SetAITargetLua);
	g_pkScript->ExposeFunction("SetAIState",	SI_P_AI::SetAIStateLua);

	// Register Constants
	g_pkScript->RegisterConstant("AISTATE_RANDOMWALK",		eAI_STATE_RANDOMWALK);
	g_pkScript->RegisterConstant("AISTATE_LOOK",				eAI_STATE_LOOKAT);
	g_pkScript->RegisterConstant("AISTATE_CHASE",			eAI_STATE_CHASE);
	g_pkScript->RegisterConstant("AISTATE_ATTACK",			eAI_STATE_ATTACK);
	g_pkScript->RegisterConstant("AISTATE_GUARD",			eAI_STATE_GUARD);
	g_pkScript->RegisterConstant("AISTATE_DEAD",				eAI_STATE_DEAD);
	g_pkScript->RegisterConstant("AISTATE_AVOID",			eAI_STATE_AVOIDING);
	g_pkScript->RegisterConstant("AISTATE_SIT",				eAI_STATE_SIT);
}

