#include "p_ai.h"
#include "../zerofpsv2/engine/p_pfpath.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../zerofpsv2/engine/entity.h"
#include "../zerofpsv2/engine/zerofps.h"
#include <iostream>

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

	m_fSeeDistance = 		10;
	m_fAttackDistance =	5;
	m_fStrikeRange = 		2;
	
	m_bWalk 		= true;
	m_fTime 		= 0;
	m_iTarget	= -1;
}

P_AI::~P_AI()
{


}


void P_AI::Init()
{
	m_fTime = m_pkZeroFps->GetTicks() + Randomf(5);

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
	
			
	switch(m_iState)
	{
		//guard
		case 4:
		{	
			//look for enemy
			int iEnemy = FindClosestEnemy(m_fSeeDistance);
			if(iEnemy != -1)
			{
				//set look att state
				m_iState = 2;
				m_iTarget = iEnemy;
			}		
		
			break;
		}
	
		//random walk
		case 1:
		{			
			if(m_bWalk)
			{
				float fRot = m_pkCharacterControl->GetYAngle();				
				fRot += Randomf(20)-10;
				//fRot +=4;
				m_pkCharacterControl->SetYAngle(fRot);								
				m_pkCharacterControl->SetControl(eUP,true);
				m_pkCharacterControl->SetControl(eCRAWL,true);											
			
				if(m_pkZeroFps->GetTicks() > m_fTime + 2)
				{
					m_bWalk = false;
					m_fTime = m_pkZeroFps->GetTicks();
				}
			}
			else
			{
				m_pkCharacterControl->SetControl(eUP,false);					
			
				if(m_pkZeroFps->GetTicks() > m_fTime + 5)
				{
					m_bWalk = true;
					m_fTime = m_pkZeroFps->GetTicks();					
				}			
			}
		
		
			//look for enemy , and attack
			int iEnemy = FindClosestEnemy(m_fAttackDistance);
			if(iEnemy != -1)
			{
				//set attack state
				m_iState = 3;
				m_iTarget = iEnemy;
				
			}
						
			break;
		}
	
		//look at
		case 2:
		{
			if(Entity* pkEnemy = m_pkEntityManager->GetEntityByID(m_iTarget))
			{
				float fDistance = pkEnemy->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
			
				if(fDistance > m_fSeeDistance)
				{
					cout<<"target went out of sight"<<endl;
					m_iState = 1;
					break;
				}
			
				//look at
				m_pkCharacterControl->RotateTowards(pkEnemy->GetWorldPosV());
				m_pkCharacterControl->SetControl(eUP,false);						
				//m_pkCharacterControl->SetControl(eCRAWL,true);						
				
				if(fDistance < m_fAttackDistance)
				{
					m_iState = 3;
					break;
				}
				
			}
			else
			{
				cout<<"target disapered"<<endl;
				m_iState = 1;
			}
			
			break;
		}
			
		//chase
		case 3:
		{
			if(Entity* pkEnemy = m_pkEntityManager->GetEntityByID(m_iTarget))
			{
				float fDistance = pkEnemy->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
			
				if(fDistance > m_fSeeDistance)
				{
					cout<<"target went out of attack/see distance"<<endl;
					m_iState = 1; //return to random walk
					break;
				}
			
				
				if(fDistance < m_fStrikeRange)
				{
					m_iState = 5;
					break;				
				}
				
				//chase
				m_pkCharacterControl->RotateTowards(pkEnemy->GetWorldPosV());
				m_pkCharacterControl->SetControl(eUP,true);
				m_pkCharacterControl->SetControl(eCRAWL,false);						
				
			
			}
			else
			{
				cout<<"target disapered"<<endl;
				m_iState = 1;
			}			
			
			break;
		}
		
		//attack
		case 5:
		{
			if(Entity* pkEnemy = m_pkEntityManager->GetEntityByID(m_iTarget))
			{
				float fDistance = pkEnemy->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
			
				if(fDistance > m_fStrikeRange)
				{
					m_iState = 3;
					break;
				}
		
				m_pkCharacterControl->RotateTowards(pkEnemy->GetWorldPosV());
				m_pkCharacterControl->SetControl(eUP,false);
				
				m_pkCharacterControl->DoEmote(1);				
			}
			else
			{
				cout<<"target disapered"<<endl;
				m_iState = 1;				
			}
			
			break;							
		}					
	}
}

void P_AI::Touch(int iID)
{
	float fRot = m_pkCharacterControl->GetYAngle();				
	//fRot += Randomf(20)-10;
	fRot +=180;
	m_pkCharacterControl->SetYAngle(fRot);								


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
		for(int i = 0;i<kPropertys.size();i++)
		{
			//skip self
			if(kPropertys[i] == m_pkCharacterProperty)
				continue;
		
			//we only care about character propertys
 			if(!kPropertys[i]->IsPropertyType("P_CharacterProperty"))
 				continue;
				
			if(P_CharacterProperty* pkCP = static_cast<P_CharacterProperty*>(kPropertys[i]))
			{
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



Property* Create_P_AI()
{
	return new P_AI;
}

void Register_P_AI(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_AI", Create_P_AI);					

	// Register Property Script Interface
// 	g_pkScript->ExposeFunction("PickupItem",	SI_P_CharacterProperty::PickupItemLua);
// 	g_pkScript->ExposeFunction("HaveItem",		SI_P_CharacterProperty::HaveItemLua);

}



