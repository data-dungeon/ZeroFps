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

P_AI::P_AI()
{
	strcpy(m_acName,"P_AI");
	m_iSide		= PROPERTY_SIDE_SERVER;
	m_iType		= PROPERTY_TYPE_NORMAL;
	m_bNetwork 	= false;
	m_iVersion	= 2;
	
	
	m_pkCharacterControl = NULL;
	m_pkCharacterProperty = NULL;
	
	m_iState = 1;

	m_fSeeDistance = 		15;
	m_fAttackDistance =	10;
	m_fStrikeRange = 		2;
	
	m_bWalk 		= true;
	m_fTime 		= 0;
	m_iTarget	= -1;
	
	m_fFindTime = 0;
}

P_AI::~P_AI()
{


}


void P_AI::Init()
{
	m_fTime = m_pkZeroFps->GetEngineTime() + Randomf(5);

}


void P_AI::Update() 
{
	if(!m_pkEntity->InActiveZone())
		return;


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
	
	switch(m_iState)
	{
		//guard
		case eAI_STATE_GUARD:
		{	
			//look for enemy
			if(m_pkZeroFps->GetEngineTime() > m_fFindTime + 1)
			{
				m_fFindTime = m_pkZeroFps->GetEngineTime();
			
				int iEnemy = FindClosestEnemy(m_fSeeDistance);
				if(iEnemy != -1)
				{
					//set look att state
					m_iState = eAI_STATE_LOOKAT;
					m_iTarget = iEnemy;
					m_pkCharacterProperty->SetCombatMode(false);
				}	
			
			}	
		
			break;
		}
	
		//random walk
		case eAI_STATE_RANDOMWALK:
		{			
			if(m_bWalk)
			{
				float fRot = m_pkCharacterControl->GetYAngle();				
				fRot += Randomf(20)-10;
				//fRot +=4;
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
					m_iState = eAI_STATE_CHASE;
					m_iTarget = iEnemy;
					
				}
			}
			
				 					
			break;
		}
	
		//look at
		case eAI_STATE_LOOKAT:
		{
			if(!ValidTarget(m_iTarget))
			{
				m_iState = 1;
				break;
			}		
		
			if(Entity* pkEnemy = m_pkEntityManager->GetEntityByID(m_iTarget))
			{
				float fDistance = pkEnemy->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
			
				if(fDistance > m_fSeeDistance)
				{
					cout<<"target went out of sight"<<endl;
					m_iState = eAI_STATE_RANDOMWALK;
					break;
				}
			
				//look at
				m_pkCharacterControl->RotateTowards(pkEnemy->GetWorldPosV());
				m_pkCharacterControl->SetControl(eUP,false);						
				//m_pkCharacterControl->SetControl(eCRAWL,true);						
				
				if(fDistance < m_fAttackDistance)
				{
					m_iState = eAI_STATE_CHASE;
					break;
				}
				
			}

			
			break;
		}
			
		//chase
		case eAI_STATE_CHASE:
		{
			if(!ValidTarget(m_iTarget))
			{
				m_iState = eAI_STATE_RANDOMWALK;
				break;
			}		
		
			if(Entity* pkEnemy = m_pkEntityManager->GetEntityByID(m_iTarget))
			{
				float fDistance = pkEnemy->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
			
				if(fDistance > m_fSeeDistance)
				{
					cout<<"target went out of attack/see distance"<<endl;
					m_iState = eAI_STATE_RANDOMWALK; //return to random walk
					break;
				}
			
				
				if(fDistance < m_fStrikeRange )
				{
					m_iState = eAI_STATE_ATTACK;
					break;				
				}
				
				//chase
				m_pkCharacterProperty->SetCombatMode(true);
				m_pkCharacterProperty->SetTarget(m_iTarget);
				
				m_pkCharacterControl->RotateTowards(pkEnemy->GetWorldPosV());
				m_pkCharacterControl->SetControl(eUP,true);
				m_pkCharacterControl->SetControl(eCRAWL,false);						
				
			
			}
	
			
			break;
		}
		
		//attack
		case eAI_STATE_ATTACK:
		{
			if(!ValidTarget(m_iTarget))
			{
				m_iState = eAI_STATE_RANDOMWALK;
				break;
			}
			
			if(Entity* pkEnemy = m_pkEntityManager->GetEntityByID(m_iTarget))
			{
				float fDistance = pkEnemy->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
			
				if(fDistance > m_fStrikeRange && m_fStrikeRange != -1)
				{
					m_iState = eAI_STATE_CHASE;
					break;
				}
						
				m_pkCharacterProperty->SetCombatMode(true);
				m_pkCharacterProperty->SetTarget(m_iTarget);
				m_pkCharacterControl->RotateTowards(pkEnemy->GetWorldPosV());
				m_pkCharacterControl->SetControl(eUP,false);
								
				//UseOffensiveSkill();
	
			}
			
			break;							
		}	
		
		//dead	
		case eAI_STATE_DEAD:
		{
			// hello, I'm dead, not much too do..not much AI
			//cout << "im so dead" << endl;
         break;							
		}
	}
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
	float fRot = m_pkCharacterControl->GetYAngle();				
	//fRot += Randomf(20)-10;
	fRot +=180;
	m_pkCharacterControl->SetYAngle(fRot);								


}

float P_AI::GetOffensiveRange()
{
	float fRange = -1;

	vector<Skill*>* kSkills =m_pkCharacterProperty->GetSkillList();
	
	for(int i =0;i<kSkills->size();i++)
	{
		if((*kSkills)[i]->m_iSkillType == eOFFENSIVE)
		{
			if((*kSkills)[i]->m_fTimeLeft == 0)
			{
				if((*kSkills)[i]->m_fRange > fRange)
					fRange = (*kSkills)[i]->m_fRange;
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
		if((*kSkills)[i]->m_iSkillType == eOFFENSIVE)
		{
			if((*kSkills)[i]->m_fTimeLeft == 0)
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
	
	
	
	//we assume that parent is a zone
	if(Entity* pkZone = GetEntity()->GetParent())
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

	int SetAIStateLua(lua_State* pkLua)
	{
		if( g_pkScript->GetNumArgs(pkLua) == 2 )
		{
			double dEntID, dAIState;
			char	acType[128];
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



