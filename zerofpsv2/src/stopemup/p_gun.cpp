#include "p_gun.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/engine_systems/propertys/p_tcs.h"
#include "../zerofpsv2/engine_systems/propertys/p_sound.h"
#include "walker.h"
#include "../zerofpsv2/basic/math.h"

P_Gun::P_Gun(): Property("P_Gun")
{
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	m_bNetwork = false;
	m_iVersion = 1;

	
	m_bFire = false;
	
	m_fAngle			= 	45;
	m_iProjectiles = 	1;
	m_fFireDelay = 	0.2;
	m_fLastFire = 		0;
	m_fBulletVel = 	20;
	m_strProjectile = "";
	m_iDamage = 		1;
	m_strSound = 		"auto_gun.wav";
	m_strDirectHitObject = "data/script/objects/bullethit.lua";
}

void P_Gun::SetSound(string strSound)
{
	if(P_Sound* pkSound = (P_Sound*)GetEntity()->GetProperty("P_Sound"))
		pkSound->StopSound(m_strSound);
		
	m_strSound = strSound;
}

void P_Gun::Update()
{
	float fT = m_pkZeroFps->GetEngineTime();

	if(m_bFire)
	{
		if(fT  > m_fLastFire + m_fFireDelay)
		{
			m_fLastFire = fT;

			FireBullet();							
		}
	}
	else
	{
		if(fT  > m_fLastFire + m_fFireDelay)
		{
			if(!m_strSound.empty())
			{
				//stop sound
				if(P_Sound* pkSound = (P_Sound*)GetEntity()->GetProperty("P_Sound"))
					pkSound->StopSound(m_strSound);			
			}
		}	
	}
}

void P_Gun::FireBullet()
{
		//play fire sound
	if(P_Sound* pkSound = (P_Sound*)GetEntity()->GetProperty("P_Sound"))
		pkSound->StartSound(m_strSound,true);

	//projectile weapon
	if(!m_strProjectile.empty())
	{
		float fAngleDif = m_fAngle / (m_iProjectiles-1);		
		float fCurrentAngle = -(m_fAngle/2.0);
		Matrix4 kRot;
		
		for(int i = 0;i < m_iProjectiles;i++)
		{		
			if(Entity* pkBullet = m_pkEntityManager->CreateEntityFromScriptInZone(m_strProjectile.c_str(),GetEntity()->GetWorldPosV(),GetEntity()->GetCurrentZone()))
			{
				pkBullet->SetVarDouble("owner",GetEntity()->GetEntityID());
				
				if(P_Tcs* pkTcs = (P_Tcs*)pkBullet->GetProperty("P_Tcs"))
				{
					kRot =  GetEntity()->GetWorldRotM();
					kRot.Rotate(0,fCurrentAngle,0);
					
					Vector3 kDir = kRot.VectorTransform(Vector3(0,0,1));
				
					pkTcs->SetLinVel(kDir*m_fBulletVel);	
				}
			}
			
			fCurrentAngle+=fAngleDif;
		}		
	}
	//direct hit
	else
	{
	
		Matrix4 kRot = GetEntity()->GetWorldRotM();
		kRot.Rotate(Math::Randomf(8)-4,Math::Randomf(8)-4,Math::Randomf(8)-4);
	
		Vector3 kDir = kRot.VectorTransform(Vector3(0,0,1));			
		Vector3 kStart = GetEntity()->GetWorldPosV();
		
		vector<Entity*> kObjects;		
		m_pkEntityManager->GetZoneEntity()->GetAllEntitys(&kObjects);

		float d;	
		Vector3 cp;
		float closest = 999999999;
		Entity* pkClosest = NULL;
		Vector3 kPickPos;
		
		for(unsigned int i=0;i<kObjects.size();i++)
		{
			if(kObjects[i] == m_pkEntity)
				continue;
					
			//is it a stoper?
			if(kObjects[i]->GetType() == "stop.lua")
				continue;
				
			//get mad property and do a linetest		
			if(P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad"))
			{
				bool bSP = false;
			
				if(kObjects[i]->GetProperty("P_Walker"))
					bSP = true;					
				
				if(mp->TestLine(kStart,kDir,bSP,bSP))
				{	
					cp = mp->GetLastColPos();
					d = kStart.DistanceTo(cp);
	
					if(d < closest)
					{						
						closest = d;
						pkClosest = kObjects[i];
						kPickPos = cp;
					}				
				}
			}				
		}
		
		//has target
		if(pkClosest)
		{
			if(P_Walker* pkWalk = (P_Walker*)pkClosest->GetProperty("P_Walker"))
			{
			
				pkWalk->Damage(m_iDamage,GetEntity()->GetEntityID());
				float fRadius = pkClosest->GetRadius();
				
				Vector3 kHitPos = pkClosest->GetWorldPosV() + Vector3(Math::Randomf(fRadius)-fRadius/2.0,-Math::Randomf(fRadius),Math::Randomf(fRadius)-fRadius/2.0);
				m_pkEntityManager->CreateEntityFromScriptInZone(m_strDirectHitObject.c_str(),kHitPos,GetEntity()->GetCurrentZone());				
			
			}
			else
				m_pkEntityManager->CreateEntityFromScriptInZone(m_strDirectHitObject.c_str(),	kPickPos,GetEntity()->GetCurrentZone());				

		}	
	
	}
}



Property* Create_P_Gun()
{
	return new P_Gun;
}





