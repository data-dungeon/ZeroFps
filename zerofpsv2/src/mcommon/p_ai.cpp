#include "p_ai.h"
#include "../zerofpsv2/engine/p_pfpath.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../zerofpsv2/engine/entity.h"
#include "../zerofpsv2/engine/zerofps.h"
#include <iostream>

#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h"

#include "p_charactercontrol.h"
#include "p_characterproperty.h"

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


P_AI::P_AI()
{
	m_pkRender=				static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			

	strcpy(m_acName,"P_AI");
	m_iSide		= PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;
	m_iType		= PROPERTY_TYPE_NORMAL|PROPERTY_TYPE_RENDER;
	m_bNetwork 	= false;
	m_iVersion	= 2;
	
	
	m_pkCharacterControl = NULL;
	m_pkCharacterProperty = NULL;
	
	m_iState				= 1;
	m_iNextState		= 0;
	m_bStateChanged	= false;


	m_fSeeDistance = 		15;
	m_fAttackDistance =	10;
	m_fStrikeRange = 		2;
	
	m_bWalk 		= true;
	m_fTime 		= 0;
	m_iTarget	= -1;
	
	m_fFindTime = 0;
	
	SetState(eAI_STATE_GUARD);
}

P_AI::~P_AI()
{


}


void P_AI::Init()
{
	m_fTime = m_pkZeroFps->GetEngineTime() + Randomf(5);

}

void P_AI::DrawCircle(float fRadius, char* szEditColor)
{
	Vector3	kVertex;
	vector<Vector3>	kVertexList;
	Vector3 kCenterPos = GetEntity()->GetIWorldPosV();

	for(int i=0; i<360; i+=(int)12.25) 
	{
		kVertex.x = float( cos(DegToRad( float(i) )) * fRadius );
		kVertex.z = float( sin(DegToRad( float(i) )) * fRadius );
		kVertex.y = 0;
		
		kVertex += kCenterPos;
		kVertexList.push_back(kVertex);
	}

	m_pkRender->DrawCircle(kVertexList, m_pkRender->GetEditColor(szEditColor) );

}

void P_AI::DrawEditor()
{
	if(!m_pkEntityManager->m_bAiShowInfo)
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
		
		m_pkCharacterControl->SetYAngle(Randomf(360));								
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
				
	m_fStrikeRange = GetOffensiveRange();
	
	Process(EVENT_Update);
}


void P_AI::Process(int iEvent)
{
	States(iEvent, m_iState);	

	// Check if state has changed and send events
	int iSaftyCount = 10;
	while(m_bStateChanged && (--iSaftyCount >= 0))
	{
		ZFAssert( iSaftyCount > 0, "AI States are unstable");
		
		m_bStateChanged = false;
		States(EVENT_Exit, m_iState);
		m_iState = m_iNextState;
		States(EVENT_Enter, m_iState);
	}
}

bool P_AI::States(int iEvent, int iState)
{
	BeginStateMachine
		State(eAI_STATE_RANDOMWALK)
			OnEnter
				m_pkCharacterProperty->DebugSet("AiState", "eAI_STATE_RANDOMWALK");
				
			OnUpdate
				if(m_bWalk)
				{
					float fRot = m_pkCharacterControl->GetYAngle();				
					fRot += Randomf(20)-10;
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
					SetState( eAI_STATE_RANDOMWALK );
					return false;
				}		
			
				if(Entity* pkEnemy = m_pkEntityManager->GetEntityByID(m_iTarget))
				{
					float fDistance = pkEnemy->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
				
					if(fDistance > m_fSeeDistance)
					{
						SetState(eAI_STATE_RANDOMWALK);
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

		State(eAI_STATE_CHASE)
			OnEnter
				m_pkCharacterProperty->DebugSet("AiState", "eAI_STATE_CHASE");
				
			OnUpdate
				if(!ValidTarget(m_iTarget))
				{
					SetState( eAI_STATE_RANDOMWALK );
					return false;
				}		
			
				if(Entity* pkEnemy = m_pkEntityManager->GetEntityByID(m_iTarget))
				{
					float fDistance = pkEnemy->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
				
					if(fDistance > m_fSeeDistance)
					{
						SetState( eAI_STATE_RANDOMWALK ); //return to random walk
						return false;
					}
				
					
					if(fDistance < m_fStrikeRange )
					{
						SetState (eAI_STATE_ATTACK );
						return false;
					}
					
					//chase
					m_pkCharacterProperty->SetCombatMode(true);
					m_pkCharacterProperty->SetTarget(m_iTarget);
					
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
					SetState(eAI_STATE_RANDOMWALK);
					return false;
				}
				
				if(Entity* pkEnemy = m_pkEntityManager->GetEntityByID(m_iTarget))
				{
					float fDistance = pkEnemy->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
				
					if(fDistance > m_fStrikeRange && m_fStrikeRange != -1)
					{
						SetState(eAI_STATE_CHASE);
						return false;
					}
							
					m_pkCharacterProperty->SetCombatMode(true);
					m_pkCharacterProperty->SetTarget(m_iTarget);
					m_pkCharacterControl->RotateTowards(pkEnemy->GetWorldPosV());
					m_pkCharacterControl->SetControl(eUP,false);
									
					//UseOffensiveSkill();
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
						SetState(eAI_STATE_LOOKAT);
						m_iTarget = iEnemy;
						m_pkCharacterProperty->SetCombatMode(false);
					}	
				}	

		State(eAI_STATE_DEAD)
			OnEnter
				m_pkCharacterProperty->DebugSet("AiState", "eAI_STATE_DEAD");

			OnUpdate
	EndStateMachine
}


bool P_AI::ValidTarget(int iTarget)
{
	if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iTarget,"P_CharacterProperty"))
		if(!pkCP->IsDead())
			return true;	

	return false;	
}

void P_AI::Touch(int iID)
{
	if(m_pkCharacterControl)
	{
	
		float fRot = m_pkCharacterControl->GetYAngle();				
		//fRot += Randomf(20)-10;
		fRot +=180;
		m_pkCharacterControl->SetYAngle(fRot);								
	}
}

float P_AI::GetOffensiveRange()
{
	float fRange = -1;

	vector<Skill*>* kSkills =m_pkCharacterProperty->GetSkillList();
	
	for(int i =0;i<kSkills->size();i++)
	{
		if((*kSkills)[i]->GetSkillType() == eOFFENSIVE)
		{
			if((*kSkills)[i]->GetTimeLeft() == 0)
			{
				if((*kSkills)[i]->GetRange() > fRange)
					fRange = (*kSkills)[i]->GetRange();
			}
		}
	}	

	return fRange;
}

void P_AI::UseOffensiveSkill()
{
	vector<Skill*>* kSkills =m_pkCharacterProperty->GetSkillList();
	
	for(int i =0;i<kSkills->size();i++)
	{
		if((*kSkills)[i]->GetSkillType() == eOFFENSIVE)
		{
			if((*kSkills)[i]->GetTimeLeft() == 0)
			{
				(*kSkills)[i]->Use(m_iTarget,Vector3(0,0,0),Vector3(0,0,0));
				return;
			}
		}
	}

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
 			if(!pkProp->IsPropertyType("P_CharacterProperty"))
 				continue;
				
			if(P_CharacterProperty* pkCP = static_cast<P_CharacterProperty*>(pkProp))
			{
				if(pkCP->IsDead())
					continue;
				//found character
				
				//is it eeevil?
				if(pkCP->GetFaction() != m_pkCharacterProperty->GetFaction())
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

vector<PropertyValues> P_AI::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);

	
	kReturn[0].kValueName = "state";
	kReturn[0].iValueType = VALUETYPE_INT;
	kReturn[0].pkValue    = (void*)&m_iState;		
	
	return kReturn;
}





bool P_AI::HandleSetValue( string kValueName, string kValue )
{

	return false;
}


void P_AI::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write(m_iState);
	
}

void P_AI::Load(ZFIoInterface* pkPackage,int iVersion)
{
	if(iVersion == 2)
	{
		pkPackage->Read(m_iState);
		
		
	}
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
		if(g_pkScript->GetNumArgs(pkLua) != 3)
			return 0;		

			
		int iCharcterID;
		int iTargetID;
		
		g_pkScript->GetArgInt(pkLua, 0,&iCharcterID);
		g_pkScript->GetArgInt(pkLua, 1,&iTargetID);
		
		if(P_AI* pkAI = (P_AI*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_AI"))
			pkAI->SetTarget(iTargetID);
	
		return 0;			
	}

/**	\fn SetAIState( Entity, iState)
		\brief Sets the current state of the AI
		\relates AI
*/
	int SetAIStateLua(lua_State* pkLua)
	{
		if( g_pkScript->GetNumArgs(pkLua) == 2 )
		{
			double dEntID, dAIState;
			g_pkScript->GetArgNumber(pkLua, 0, &dEntID);		
			g_pkScript->GetArgNumber(pkLua, 1, &dAIState);				
	
			Entity* pkObj = g_pkObjMan->GetEntityByID(dEntID);
	
			if ( !pkObj )
				return 0;
	
			// Get P_AI
			if(P_AI* pkAI = (P_AI*)pkObj->GetProperty("P_AI"))
				pkAI->SetState(int(dAIState));
	
			return 0;
		}
	
		return 0;
	}	
}



Property* Create_P_AI()
{
	return new P_AI;
}

void Register_P_AI(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_AI", Create_P_AI);					

	// Register Property Script Interface
 	g_pkScript->ExposeFunction("SetAITarget",	SI_P_AI::SetAITargetLua);
	g_pkScript->ExposeFunction("SetAIState",	SI_P_AI::SetAIStateLua);
}



