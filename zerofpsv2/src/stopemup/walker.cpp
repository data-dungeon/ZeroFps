#include "walker.h"
#include "../mcommon/p_charactercontrol.h"
#include "stopemup.h"
#include "p_player.h"

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
	m_fFindNewTargetTime = 0;
	m_iGoalID = m_pkStopEmUp->GetGoalEnt();
	
			
	//setup life depending on current level and number of players	
	float fPlayers = m_pkStopEmUp->GetPlayers();
	float fLevel =  m_pkStopEmUp->GetLevel();
	
	fPlayers *= 0.70;	
	if(fPlayers < 1)
		fPlayers = 1;
	
	fLevel *= 0.35;
	if(fLevel < 1)
		fLevel = 1;
			
	m_iMaxLife = (5 * fLevel) * fPlayers ;
	m_iLife = m_iMaxLife;
		
	
	
	//if abow level 20 mak em bigger
	if(m_pkStopEmUp->GetLevel() >= 20)
		m_bSetSize = true;
	else
		m_bSetSize = false;
}

void P_Walker::Update()
{
	if(m_bSetSize)
	{
		m_bSetSize = false;
		
		if(P_Mad* pkMad = (P_Mad*)GetEntity()->GetProperty("P_Mad"))
		{
			pkMad->SetScale(2);
		}
		
		if(P_Tcs* pkTcs = (P_Tcs*)GetEntity()->GetProperty("P_Tcs"))
		{
			pkTcs->SetRadius(0.8);
			pkTcs->SetLegLength(1.1);
		}
	}
	
	
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
			
				//reset target every 4 second
				if(m_pkZeroFps->GetTicks() > m_fFindNewTargetTime + 4)
				{
					m_iTarget = -1;				
				}
			
				//find target if there is none
				if(m_iTarget == -1)
				{
					m_fFindNewTargetTime = m_pkZeroFps->GetTicks();
					m_iTarget = ClosestPlayer();
					
					//is there a goal,
					if(m_iGoalID != -1)
					{
						if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iTarget))
						{
							//if player is farther away than 4 go for the goal
							if(pkEnt->GetWorldPosV().DistanceTo(GetEntity()->GetWorldPosV()) > 6)
								m_iTarget = m_iGoalID;						
						}
						else
							m_iTarget = m_iGoalID;																
					}
				}
				
				//folow target				
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
				{
					m_iTarget = -1;
					pkCC->SetControl(eUP,false);					
				}
				
				break;
			}

		}
	}
}

int P_Walker::ClosestPlayer()
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

	
	return iID;
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
	static int iLastHit = -1;
	
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
				
						Paralize(0.2);
				
					}
				}
				
			}		
		}
	
		//hit by bullet
		if(pkEnt->GetType() == "bullet.lua")
		{
			if(iID == iLastHit)
				return;			
			iLastHit = iID;
				
			int iOwner = pkEnt->GetVarDouble("owner");
		
			Damage(30,iOwner);
			m_pkEntityManager->Delete(pkEnt);
		}
	}
}


void P_Walker::Damage(int iDmg,int iKiller)
{
	//already dead
	if(m_iLife <= 0)
		return;
	
	//Paralize(0.1);
	
	m_iLife-=iDmg;
				
	if(m_iLife <= 0)
	{
		//create death object
		m_pkEntityManager->CreateEntityFromScriptInZone("data/script/objects/walkerdeath.lua",	GetEntity()->GetWorldPosV(),GetEntity()->GetCurrentZone());				
		
		//create bouns
		if(iKiller != -1)		
			CreateBonus();
		
		//delete walker
		m_pkEntityManager->Delete(GetEntity());			
		
		
		//find killer and give him some bonus 
		if(P_Player* pkPlayer = (P_Player*)m_pkEntityManager->GetPropertyFromEntityID(iKiller,"P_Player"))
		{
			pkPlayer->AddScore(m_iMaxLife);
			pkPlayer->AddKill();
		}
	}
}

void P_Walker::CreateBonus()
{
	//if( m_pkStopEmUp->GetLevel() < 4)
	//	return;
	
	int i = Randomi(5);

	if(i <= 0)
	{
		Entity* pkEnt = m_pkEntityManager->CreateEntityFromScriptInZone("data/script/objects/powerup.lua",	GetEntity()->GetWorldPosV() ,GetEntity()->GetCurrentZone());				

		if(P_Tcs* pkTcs = (P_Tcs*)pkEnt->GetProperty("P_Tcs"))
			pkTcs->ApplyImpulsForce(Vector3(0,5,0));
	}
}

Property* Create_P_Walker()
{
	return new P_Walker;
}



