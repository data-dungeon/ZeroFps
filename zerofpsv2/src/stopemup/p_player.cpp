#include "p_player.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/engine_systems/propertys/p_sound.h"

P_Player::P_Player()
{

	strcpy(m_acName,"P_Player");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	m_bNetwork = true;
	m_iVersion = 1;

	
	m_iEnergy		= 100;
	m_iMaxEnergy	= 100;
	m_iScore			= 0;	
	m_fInv			= -1;
	m_strGunName	= "Machine Gun";
}

void P_Player::Touch(int iID)
{
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(iID))
	{
		//check if invunrable
		if(m_fInv != -1)
		{
			if(m_pkZeroFps->GetTicks() > m_fInv + 0.5)
				m_fInv = -1;
			else
				return;
		}
	
		if(pkEnt->GetType() == "walker.lua")
		{		
			//play hit sound
			if(P_Sound* pkSound = (P_Sound*)GetEntity()->GetProperty("P_Sound"))
				pkSound->StartSound("data/sound/hit.wav",false);
		
				
			Damage(5);
		}
	}
}

void P_Player::Damage(int iDmg)
{
	SetNetUpdateFlag(true);
	
	
	m_iEnergy-=iDmg;

	//set inv time
	m_fInv = m_pkZeroFps->GetTicks();
	
	if(m_iEnergy <= 0 )
	{
		cout<<"player died"<<endl;
		m_pkEntityManager->CreateEntityFromScriptInZone("data/script/objects/playerdeath.lua",	GetEntity()->GetWorldPosV(),GetEntity()->GetCurrentZone());				
		m_pkEntityManager->Delete(GetEntity());		
			
	}
}

void P_Player::PackTo( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Write(m_iEnergy);
	pkNetPacket->Write(m_iMaxEnergy);
	pkNetPacket->Write(m_iScore);
	pkNetPacket->Write_Str(m_strGunName);
	

	SetNetUpdateFlag(iConnectionID,false);
} 

void P_Player::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Read(m_iEnergy);
	pkNetPacket->Read(m_iMaxEnergy);
	pkNetPacket->Read(m_iScore);
	pkNetPacket->Read_Str(m_strGunName);
}



Property* Create_P_Player()
{
	return new P_Player;
}