#include "p_ai.h"
#include "../zerofpsv2/engine/p_pfpath.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../zerofpsv2/engine/entity.h"
#include "../zerofpsv2/engine/zerofps.h"
#include <iostream>

#include "p_charactercontrol.h"

using namespace std;

P_AI::P_AI()
{
	strcpy(m_acName,"P_AI");
	m_iSide		= PROPERTY_SIDE_SERVER;
	m_iType		= PROPERTY_TYPE_NORMAL;
	m_bNetwork 	= false;
	m_iVersion	= 1;
	
	
	m_pkCharacterControl = NULL;
	
	m_iState = 1;
	
	
	m_bWalk = true;
	m_fTime = 0;
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


	if(!m_pkCharacterControl)
	{
		m_pkCharacterControl = (P_CharacterControl*)m_pkEntity->GetProperty("P_CharacterControl");
		
		m_pkCharacterControl->SetYAngle(Randomf(360));								
	}
		
	switch(m_iState)
	{
		case 1:
		{			
			if(m_bWalk)
			{
				float fRot = m_pkCharacterControl->GetYAngle();				
				fRot += Randomf(20)-10;
				//fRot +=4;
				m_pkCharacterControl->SetYAngle(fRot);								
				m_pkCharacterControl->SetControl(eUP,true);					
			
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


vector<PropertyValues> P_AI::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);

	return kReturn;
}





bool P_AI::HandleSetValue( string kValueName, string kValue )
{

	return false;
}


// void P_AI::Save(ZFIoInterface* pkPackage)
// {
// }
// 
// 
// void P_AI::Load(ZFIoInterface* pkPackage,int iVersion)
// {
// }
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



