#include "p_player.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/engine_systems/propertys/p_sound.h"
#include "stopemup.h"

P_Player::P_Player()
{
	m_pkStopEmUp = (StopEmUp*)(g_ZFObjSys.GetObjectPtr("Application"));

	strcpy(m_acName,"P_Player");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	m_bNetwork = true;
	m_iVersion = 1;

	
	m_bSecondary	= false;
	m_fSecondaryTime = 0;
	
	m_iEnergy		= 100;
	m_iMaxEnergy	= 100;
	m_iScore			= 0;
	m_iKills			= 0;
	m_fInv			= -1;
	m_strGunName	= "Machine Gun";
	m_iStopers		= 0;
	
}

void P_Player::Update()
{
	if(m_bSecondary)
	{
		//add box
		if(m_iStopers > 0)
		{			
			if(m_pkZeroFps->GetEngineTime() > m_fSecondaryTime + 1)
			{			 
				m_fSecondaryTime = m_pkZeroFps->GetEngineTime();
					
				m_pkEntityManager->CreateEntityFromScriptInZone("data/script/objects/stop.lua",GetEntity()->GetWorldPosV()+Vector3(0,-0.6,0),-1);
			
				m_iStopers--;
				ResetAllNetUpdateFlags();			
			}
		}
	}
}

void P_Player::Touch(int iID)
{
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(iID))
	{
	
		//check if invunrable
		if(m_fInv != -1)
		{
			if(m_pkZeroFps->GetEngineTime() > m_fInv + 0.5)
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
	m_fInv = m_pkZeroFps->GetEngineTime();
	
	if(m_iEnergy <= 0 )
	{
		cout<<"player died"<<endl;
		m_pkEntityManager->CreateEntityFromScriptInZone("data/script/objects/playerdeath.lua",	GetEntity()->GetWorldPosV(),GetEntity()->GetCurrentZone());				
		
		//goal game or not
		if(m_pkStopEmUp->GetGoalEnt() == -1)		
			m_pkEntityManager->Delete(GetEntity());		
		else
		{
			//set player att 0 0 0  and remove some life
			GetEntity()->SetWorldPosV(Vector3(0,0,0));
			m_iEnergy		= 100;
			m_iMaxEnergy	= 100;
			m_iStopers		= 0;
			
			//score 
			m_iScore 		-= 1000;
			if(m_iScore < 0)
				m_iScore  =0;
									
			m_pkStopEmUp->RemoveLife();
			m_pkStopEmUp->RemoveLife();
		}			
	}
}

void P_Player::PackTo( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Write(m_iEnergy);
	pkNetPacket->Write(m_iMaxEnergy);
	pkNetPacket->Write(m_iScore);
	pkNetPacket->Write(m_iKills);
	pkNetPacket->Write(m_iStopers);
	pkNetPacket->Write_Str(m_strGunName);
	

	SetNetUpdateFlag(iConnectionID,false);
} 

void P_Player::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Read(m_iEnergy);
	pkNetPacket->Read(m_iMaxEnergy);
	pkNetPacket->Read(m_iScore);
	pkNetPacket->Read(m_iKills);
	pkNetPacket->Read(m_iStopers);
	pkNetPacket->Read_Str(m_strGunName);
}



Property* Create_P_Player()
{
	return new P_Player;
}