#include "walker.h"
#include "../mcommon/p_charactercontrol.h"
#include "stopemup.h"

P_Walker::P_Walker()
{
	m_pkStopEmUp = (StopEmUp*)(g_ZFObjSys.GetObjectPtr("Application"));

	strcpy(m_acName,"P_Walker");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	m_bNetwork = false;
	m_iVersion = 1;

	m_iState = eKILL_PLAYERS;
	
	m_fParaTime = -1;
	m_iTarget = -1;

	
	//setup life depending on current level and number of players	
	int iPlayers = m_pkStopEmUp->GetPlayers();
	int iLevel =  m_pkStopEmUp->GetLevel();
	
	if(iPlayers < 1)
		iPlayers = 1;
	
	if(iLevel < 1)
		iLevel = 1;
	
	m_iLife = (20 * iLevel) * iPlayers;
}

void P_Walker::Update()
{

	//check if paralized
	if(m_fParaTime != -1)
	{
		if(m_pkZeroFps->GetTicks() > m_fParaTime + m_fParaLength)	
			m_fParaTime = -1;	
		else
			return;
	}
				
	if(P_CharacterControl* pkCC = (P_CharacterControl*)GetEntity()->GetProperty("P_CharacterControl"))
	{
	
		
		switch(m_iState)
		{
			case eRANDOM_WALK:
			{
				float fRot = pkCC->GetYAngle();
				
				fRot += Randomf(10)-5;
				pkCC->SetYAngle(fRot);
				
				
				pkCC->SetControl(eUP,true);
			
				break;
			}
		
			case eSLEEP:
			{
				pkCC->SetControl(eUP,false);
				pkCC->SetControl(eDOWN,false);
				pkCC->SetControl(eLEFT,false);
				pkCC->SetControl(eRIGHT,false);
			
				break;
			}
			
			case eKILL_PLAYERS:
			{
			
				//find target if there is none
				if(m_iTarget == -1)
				{
					vector<Entity*> kEnts;
					m_pkEntityManager->GetZoneEntity()->GetAllEntitys(&kEnts);
					
					float fClosest = 999999999;
					int iID = -1;
					
					for(int i = 0;i<kEnts.size();i++)
					{
						if(kEnts[i]->GetType() == "stopemupplayer.lua")
						{
							float d = GetEntity()->GetWorldPosV().DistanceTo(kEnts[i]->GetWorldPosV());
							if(d < fClosest)
							{
								fClosest = d;
								iID = kEnts[i]->GetEntityID();
							}
						}
					}
					
					m_iTarget = iID;					
				}
				
				if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iTarget))
				{
					Vector3 kTargetPos = pkEnt->GetWorldPosV();
				
					Vector3 kDir = kTargetPos - GetEntity()->GetWorldPosV();
					kDir.Normalize();
					kDir.y = 0;
					
					float fAngle = Vector3(0,0,1).Angle(kDir);
					fAngle = -RadToDeg(fAngle);
					
					if(kDir.x > 0)
						fAngle = fabs(fAngle);
						
					pkCC->SetYAngle(fAngle);
					pkCC->SetControl(eUP,true);					
					
				}
				else
					m_iTarget = -1;
				
				break;
			}	
		}
	}
}

void P_Walker::Paralize(float fTime )
{

	//paralize
	m_fParaTime = m_pkZeroFps->GetTicks();
	m_fParaLength = fTime;

	if(P_CharacterControl* pkCC = (P_CharacterControl*)GetEntity()->GetProperty("P_CharacterControl"))
	{	
		pkCC->SetControl(eUP,false);
		pkCC->SetControl(eDOWN,false);
		pkCC->SetControl(eLEFT,false);
		pkCC->SetControl(eRIGHT,false);			
	}
}

void P_Walker::Touch(int iID)
{
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(iID))
	{
		//collide with other walker
		if(pkEnt->GetType() == "walker.lua")
		{
			if(m_fParaTime == -1)
			{
				//check if im farther away from target then the other
				if(Entity* pkTarget = m_pkEntityManager->GetEntityByID(m_iTarget))
				{
					if(pkTarget->GetWorldPosV().DistanceTo(GetEntity()->GetWorldPosV()) > pkTarget->GetWorldPosV().DistanceTo(pkEnt->GetWorldPosV()))
					{
				
						Paralize(0.25);
				
					}
				}
				
			}		
		}
	
		//hit by bullet
		if(pkEnt->GetType() == "bullet.lua")
		{
			Damage(1);
			m_pkEntityManager->Delete(pkEnt);
		}
	}
}


void P_Walker::Damage(int iDmg)
{
	Paralize(0.5);
	
	m_iLife--;
				
	if(m_iLife <= 0)
	{
		//create death object
		m_pkEntityManager->CreateEntityFromScriptInZone("data/script/objects/walkerdeath.lua",	GetEntity()->GetWorldPosV(),GetEntity()->GetCurrentZone());				
		
		//delete walker
		m_pkEntityManager->Delete(GetEntity());						
	}

}

Property* Create_P_Walker()
{
	return new P_Walker;
}



