#include "p_powerup.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "p_player.h"
#include "p_gun.h"


P_Powerup::P_Powerup()
{
	strcpy(m_acName,"P_Powerup");
	m_iType=0;
	m_iSide=0;
	m_bNetwork = false;
	m_iVersion = 1;
	
	
	m_iPowerupType = Randomi(3);
	cout<<"created bonus type:"<<m_iPowerupType<<endl;
}

void P_Powerup::Touch(int iID)
{
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(iID))
	{
		//collide with player
		if(pkEnt->GetType() == "stopemupplayer.lua")
		{
			//player got powerup		
			m_pkEntityManager->Delete(GetEntity());

					
			if(P_Player* pkPlayer = (P_Player*)m_pkEntityManager->GetPropertyFromEntityID(iID,"P_Player"))
			{
				if(P_Gun* pkGun = (P_Gun*)m_pkEntityManager->GetPropertyFromEntityID(iID,"P_Gun"))
				{
					
					switch(m_iPowerupType)
					{
						case 0:
						{
							pkGun->m_iDamage = 1;
							pkGun->m_strDirectHitObject = "/data/script/objects/bullethit.lua";
							pkGun->m_strProjectile = "";
							pkGun->m_fFireDelay = 0.2;
							pkPlayer->m_strGunName = "Machine Gun";
							pkPlayer->SetNetUpdateFlag(true);						
							break;
						}
												
						case 1:
						{
							pkGun->m_iDamage = 10;
							pkGun->m_strDirectHitObject = "/data/script/objects/ybergun-bullethit.lua";
							pkGun->m_strProjectile = "";
							pkGun->m_fFireDelay = 0.1;
							pkPlayer->m_strGunName = "Yber Gun";
							pkPlayer->SetNetUpdateFlag(true);
							break;
						}										
						
						case 2:
						{
							pkGun->m_strProjectile = "/data/script/objects/bullet.lua";
							pkGun->m_fFireDelay = 0.4;
							pkGun->m_fBulletVel = 20;
							pkPlayer->m_strGunName = "Flash Gun";							
							pkPlayer->SetNetUpdateFlag(true);												
						
							break;
						}
					}						
						
				}
				else
					cout<<"ERROR: playering missing P_gun"<<endl;
			}
			else
				cout<<"ERROR: player missing p_player"<<endl;
		}
	}
}

Property* Create_P_Powerup()
{
	return new P_Powerup;
}




