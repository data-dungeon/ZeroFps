#include "p_powerup.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "p_player.h"
#include "p_gun.h"
#include "stopemup.h"


P_Powerup::P_Powerup()
{
	m_pkStopEmUp = (StopEmUp*)(g_ZFObjSys.GetObjectPtr("Application"));
	m_pkRender=		(Render*)(g_ZFObjSys.GetObjectPtr("Render"));	
	
	strcpy(m_acName,"P_Powerup");
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	m_bNetwork = true;
	m_iVersion = 1;
	
	
	//text
	//setup material
	m_pkTextMaterial = new ZMaterial;
	m_pkTextMaterial->GetPass(0)->m_kTUs[0]->SetRes("data/textures/text/fetfont.tga");
	m_pkTextMaterial->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
	m_pkTextMaterial->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;		
	m_pkTextMaterial->GetPass(0)->m_bLighting = 				false;		
	m_pkTextMaterial->GetPass(0)->m_bColorMaterial = 		true;
	m_pkTextMaterial->GetPass(0)->m_kVertexColor =			Vector3(0,1,0);
	m_pkTextMaterial->GetPass(0)->m_bFog = 					true;		
	m_pkTextMaterial->GetPass(0)->m_bAlphaTest =				true;		
	m_pkTextMaterial->GetPass(0)->m_bDepthTest = 			true;

	//setup font
	m_pkFont = new ZGuiFont("CharacterFont");
	m_pkFont->Create("/data/textures/text/fetfont.fnt",-1);		
	
	
	//blub
	m_strPowerupName = "PowerUp";
	
	int iLevel = m_pkStopEmUp->GetLevel();
	m_iPowerupType = -1;
	
	//randomize powerup
	while(m_iPowerupType == -1)
	{
		int iRand = Randomi(10);	
		
		//check if type is level aproved
		switch(iRand)
		{
			case 0:
				m_iPowerupType = iRand;
				m_strPowerupName = "Health";
				break;			
			
			case 1:
				if(iLevel >= 6)
				{
					m_iPowerupType = iRand;				
					m_strPowerupName = "Yber Gun";	
				}	
				break;
		
			case 2:
				if(iLevel >= 8)
				{
					m_iPowerupType = iRand;
					m_strPowerupName = "Flash Gun";	
				}						
				break;

			case 3:
				if(iLevel >= 3)
				{
					m_iPowerupType = iRand;
					m_strPowerupName = "Mini Gun";	
				}	
				break;
								
		}
	}
		
	
	cout<<"created bonus type:"<<m_iPowerupType<<endl;
		
}

P_Powerup::~P_Powerup()
{
	delete m_pkTextMaterial;
	delete m_pkFont;	
}

void P_Powerup::Update()
{
	float fDistance = m_pkZeroFps->GetCam()->GetRenderPos().DistanceTo(GetEntity()->GetWorldPosV());
	if(fDistance < 20)
	{
		float fScale = 0.04 * fDistance;
		
		m_pkRender->PrintBillboard(m_pkZeroFps->GetCam()->GetRotM(),GetEntity()->GetIWorldPosV()+
						Vector3(0,0.5,0),fScale,m_strPowerupName,m_pkTextMaterial,m_pkFont,true);													
	}	

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
						//health
						case 0:
						{
							pkPlayer->m_iEnergy = pkPlayer->m_iMaxEnergy;
							
							pkPlayer->SetNetUpdateFlag(true);						
							break;
						}

						//yber gun												
						case 1:
						{
							pkGun->m_iDamage = 5;
							pkGun->m_strDirectHitObject = "/data/script/objects/ybergun-bullethit.lua";
							pkGun->m_strProjectile = "";
							pkGun->m_fFireDelay = 0.1;
							pkPlayer->m_strGunName = "Yber Gun";
							pkPlayer->SetNetUpdateFlag(true);
							break;
						}										
						
						//flash gun
						case 2:
						{
							pkGun->m_strProjectile = "/data/script/objects/bullet.lua";
							pkGun->m_fFireDelay = 0.4;
							pkGun->m_fBulletVel = 20;
							pkPlayer->m_strGunName = "Flash Gun";							
							pkPlayer->SetNetUpdateFlag(true);												
						
							break;
						}
					
						//mini gun
						case 3:
						{
							
							pkGun->m_iDamage = 2;
							pkGun->m_strDirectHitObject = "/data/script/objects/bullethit.lua";
							pkGun->m_strProjectile = "";
							pkGun->m_fFireDelay = 0.1;
							pkPlayer->m_strGunName = "Mini Gun";
							
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

void P_Powerup::PackTo( NetPacket* pkNetPacket, int iConnectionID ) 
{
	pkNetPacket->Write_Str(m_strPowerupName);

	SetNetUpdateFlag(iConnectionID,false);
	
}

void P_Powerup::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Read_Str(m_strPowerupName);
}

Property* Create_P_Powerup()
{
	return new P_Powerup;
}




